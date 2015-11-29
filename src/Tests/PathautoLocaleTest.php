<?php

/**
 * @file
 * Contains \Drupal\pathauto\Tests\PathautoLocaleTest.
 */

namespace Drupal\pathauto\Tests;

use Drupal\Core\Language\Language;
use Drupal\Core\Language\LanguageInterface;
use Drupal\language\Entity\ConfigurableLanguage;
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
  public static $modules = array('node', 'pathauto', 'locale');

  /**
   * Admin user.
   *
   * @var \Drupal\user\UserInterface
   */
  protected $adminUser;

  /**
   * Test that when an English node is updated, its old English alias is
   * updated and its newer French alias is left intact.
   */
  function testLanguageAliases() {

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
}

