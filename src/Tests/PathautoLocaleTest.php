<?php

/**
 * @file
 * Contains \Drupal\pathauto\Tests\PathautoLocaleTest.
 */

namespace Drupal\pathauto\Tests;

use Drupal\Core\Language\Language;
use Drupal\Core\Language\LanguageInterface;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\Core\Url;
use Drupal\language\Entity\ConfigurableLanguage;
use Drupal\language\Entity\ContentLanguageSettings;
use Drupal\pathauto\PathautoState;
use Drupal\simpletest\WebTestBase;

/**
 * Test pathauto functionality with localization and translation.
 *
 * @group pathauto
 */
class PathautoLocaleTest extends WebTestBase {

  use PathautoTestHelperTrait;

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('node', 'pathauto', 'locale', 'content_translation');

  /**
   * {@inheritdoc}
   */
  protected function setUp() {
    parent::setUp();

    // Create Article node types.
    if ($this->profile != 'standard') {
      $this->drupalCreateContentType(array('type' => 'article', 'name' => 'Article'));
    }
  }

  /**
   * Test that when an English node is updated, its old English alias is
   * updated and its newer French alias is left intact.
   */
  function testLanguageAliases() {

    $this->createPattern('node', '/content/[node:title]');

    // Add predefined French language.
    ConfigurableLanguage::createFromLangcode('fr')->save();

    $node = array(
      'title' => 'English node',
      'langcode' => 'en',
      'path' => array(array(
        'alias' => '/english-node',
        'pathauto' => FALSE,
      )),
    );
    $node = $this->drupalCreateNode($node);
    $english_alias = \Drupal::service('path.alias_storage')->load(array('alias' => '/english-node', 'langcode' => 'en'));
    $this->assertTrue($english_alias, 'Alias created with proper language.');

    // Also save a French alias that should not be left alone, even though
    // it is the newer alias.
    $this->saveEntityAlias($node, '/french-node', 'fr');

    // Add an alias with the soon-to-be generated alias, causing the upcoming
    // alias update to generate a unique alias with the '-0' suffix.
    $this->saveAlias('/node/invalid', '/content/english-node', Language::LANGCODE_NOT_SPECIFIED);

    // Update the node, triggering a change in the English alias.
    $node->path->pathauto = PathautoState::CREATE;
    $node->save();

    // Check that the new English alias replaced the old one.
    $this->assertEntityAlias($node, '/content/english-node-0', 'en');
    $this->assertEntityAlias($node, '/french-node', 'fr');
    $this->assertAliasExists(array('pid' => $english_alias['pid'], 'alias' => '/content/english-node-0'));

    // Create a new node with the same title as before but without
    // specifying a language.
    $node = $this->drupalCreateNode(array('title' => 'English node', 'langcode' => LanguageInterface::LANGCODE_NOT_SPECIFIED));

    // Check that the new node had a unique alias generated with the '-1'
    // suffix.
    $this->assertEntityAlias($node, '/content/english-node-1', LanguageInterface::LANGCODE_NOT_SPECIFIED);
  }

  /**
   * Test that patterns work on multilingual content.
   */
  function testLanguagePatterns() {
    $this->drupalLogin($this->rootUser);

    // Add French language.
    ConfigurableLanguage::createFromLangcode('fr')->save();

    // Enable content translation on articles.
    \Drupal::service('content_translation.manager')->setEnabled('node', 'article', TRUE);
    drupal_static_reset();
    \Drupal::entityManager()->clearCachedDefinitions();
    \Drupal::service('router.builder')->rebuild();
    \Drupal::service('entity.definition_update_manager')->applyUpdates();

    // Create a pattern for English articles.
    $pattern = $this->createPattern('node', '/the-articles/[node:title]');
    $pattern->addSelectionCondition([
      'id' => 'entity_bundle:node',
      'bundles' => ['article' => 'article'],
      'negate' => FALSE,
      'context_mapping' => ['node' => 'node'],
    ]);
    $language_mapping = 'node:langcode:language';
    $pattern->addSelectionCondition([
      'id' => 'language',
      'langcodes' => ['en' => 'en'],
      'negate' => FALSE,
      'context_mapping' => ['language' => $language_mapping],
    ]);
    $new_definition = new ContextDefinition('language', 'Language');
    $new_context = new Context($new_definition);
    $pattern->addContext($language_mapping, $new_context);
    $pattern->save();

    // Create a pattern for French articles.
    $pattern = $this->createPattern('node', '/les-articles/[node:title]');
    $pattern->addSelectionCondition([
      'id' => 'entity_bundle:node',
      'bundles' => ['article' => 'article'],
      'negate' => FALSE,
      'context_mapping' => ['node' => 'node'],
    ]);
    $language_mapping = 'node:langcode:language';
    $pattern->addSelectionCondition([
      'id' => 'language',
      'langcodes' => ['fr' => 'fr'],
      'negate' => FALSE,
      'context_mapping' => ['language' => $language_mapping],
    ]);
    $new_definition = new ContextDefinition('language', 'Language');
    $new_context = new Context($new_definition);
    $pattern->addContext($language_mapping, $new_context);
    $pattern->save();

    // Create a node and its translation. Assert aliases.
    $edit = array(
      'title[0][value]' => 'English node',
    );
    $this->drupalPostForm('node/add/article', $edit, t('Save and publish'));
    $this->drupalGet('node/1/edit');
    $english_node = $this->drupalGetNodeByTitle('English node');
    $this->assertAlias('/node/' . $english_node->id(), '/the-articles/english-node', 'en');

    $add_translation_url = Url::fromRoute('entity.node.content_translation_add', ['node' => $english_node->id(), 'source' => 'en', 'target' => 'fr']);
    $edit = array(
      'title[0][value]' => 'French node',
    );
    $this->drupalPostForm($add_translation_url, $edit, t('Save and keep published (this translation)'));
    $this->rebuildContainer();
    $english_node = $this->drupalGetNodeByTitle('English node');
    $french_node = $english_node->getTranslation('fr');
    $this->assertAlias('/node/' . $french_node->id(), '/les-articles/french-node', 'fr');

    // Bulk delete and Bulk generate patterns. Assert aliases.
    $this->deleteAllAliases();
    // Bulk create aliases.
    $edit = array(
      'update[canonical_entities:node]' => TRUE,
    );
    $this->drupalPostForm('admin/config/search/path/update_bulk', $edit, t('Update'));
    $this->assertText('Generated 1 URL alias.');
    $this->assertAlias('/node/' . $english_node->id(), '/the-articles/english-node', 'en');
    $this->assertAlias('/node/' . $french_node->id(), '/les-articles/french-node', 'fr');
  }

}
