<?php

namespace Drupal\pathauto\Tests\Pathauto;

use Drupal\Core\Language\Language;
use Drupal\Component\Utility\String;

/**
 * Unit tests for Pathauto functions.
 */
class PathautoUnitTest extends PathautoTestHelper {
  public static function getInfo() {
    return array(
      'name' => 'Pathauto unit tests',
      'description' => 'Unit tests for Pathauto functions.',
      'group' => 'Pathauto',
      // 'dependencies' => array('token'),
    );
  }

  public function setUp(array $modules = array()) {
    parent::setUp($modules);
    module_load_include('inc', 'pathauto');
  }

  /**
   * Test _pathauto_get_schema_alias_maxlength().
   */
  public function testGetSchemaAliasMaxLength() {
    $this->assertIdentical(_pathauto_get_schema_alias_maxlength(), 255);
  }

  /**
   * Test pathauto_pattern_load_by_entity().
   */
  public function testPatternLoadByEntity() {

    $tests = array(
      array(
        'entity' => 'node',
        'bundle' => 'story',
        'language' => 'fr',
        'expected' => 'story/[node:title]',
      ),
      array(
        'entity' => 'node',
        'bundle' => 'story',
        'language' => 'en',
        'expected' => 'story/en/[node:title]',
      ),
      array(
        'entity' => 'node',
        'bundle' => 'story',
        'language' => Language::LANGCODE_NOT_SPECIFIED,
        'expected' => 'story/[node:title]',
      ),
      array(
        'entity' => 'node',
        'bundle' => 'page',
        'language' => 'en',
        'expected' => 'content/[node:title]',
      ),
      array(
        'entity' => 'user',
        'bundle' => 'user',
        'language' => Language::LANGCODE_NOT_SPECIFIED,
        'expected' => 'users/[user:name]',
      ),
      array(
        'entity' => 'invalid-entity',
        'bundle' => '',
        'language' => Language::LANGCODE_NOT_SPECIFIED,
        'expected' => '',
      ),
    );
    foreach ($tests as $test) {
      $actual = pathauto_pattern_load_by_entity($test['entity'], $test['bundle'], $test['language']);
      $this->assertIdentical($actual, $test['expected'], t("pathauto_pattern_load_by_entity('@entity', '@bundle', '@language') returned '@actual', expected '@expected'", array(
        '@entity' => $test['entity'],
        '@bundle' => $test['bundle'],
        '@language' => $test['language'],
        '@actual' => $actual,
        '@expected' => $test['expected'],
      )));
    }
  }

  /**
   * Test pathauto_cleanstring().
   */
  public function testCleanString() {

    $config = \Drupal::configFactory()->get('pathauto.settings');

    $tests = array();
    $config->set('ignore_words', ', in, is,that, the  , this, with, ');
    $config->set('max_component_length', 35);
    $config->save();

    // Test the 'ignored words' removal.
    $tests['this'] = 'this';
    $tests['this with that'] = 'this-with-that';
    $tests['this thing with that thing'] = 'thing-thing';

    // Test length truncation and duplicate separator removal.
    $tests[' - Pathauto is the greatest - module ever in Drupal history - '] = 'pathauto-greatest-module-ever';

    // Test that HTML tags are removed.
    $tests['This <span class="text">text</span> has <br /><a href="http://example.com"><strong>HTML tags</strong></a>.'] = 'text-has-html-tags';
    $tests[String::checkPlain('This <span class="text">text</span> has <br /><a href="http://example.com"><strong>HTML tags</strong></a>.')] = 'text-has-html-tags';

    foreach ($tests as $input => $expected) {
      $output = pathauto_cleanstring($input);
      $this->assertEqual($output, $expected, t("pathauto_cleanstring('@input') expected '@expected', actual '@output'", array(
        '@input' => $input,
        '@expected' => $expected,
        '@output' => $output,
      )));
    }
  }

  /**
   * Test pathauto_clean_alias().
   */
  public function testCleanAlias() {
    $tests = array();
    $tests['one/two/three'] = 'one/two/three';
    $tests['/one/two/three/'] = 'one/two/three';
    $tests['one//two///three'] = 'one/two/three';
    $tests['one/two--three/-/--/-/--/four---five'] = 'one/two-three/four-five';
    $tests['one/-//three--/four'] = 'one/three/four';

    foreach ($tests as $input => $expected) {
      $output = pathauto_clean_alias($input);
      $this->assertEqual($output, $expected, t("pathauto_clean_alias('@input') expected '@expected', actual '@output'", array(
        '@input' => $input,
        '@expected' => $expected,
        '@output' => $output,
      )));
    }
  }

  /**
   * Test pathauto_path_delete_multiple().
   */
  public function testPathDeleteMultiple() {
    $this->saveAlias('node/1', 'node-1-alias');
    $this->saveAlias('node/1/view', 'node-1-alias/view');
    $this->saveAlias('node/1', 'node-1-alias-en', 'en');
    $this->saveAlias('node/1', 'node-1-alias-fr', 'fr');
    $this->saveAlias('node/2', 'node-2-alias');

    pathauto_path_delete_all('node/1');
    $this->assertNoAliasExists(array('source' => "node/1"));
    $this->assertNoAliasExists(array('source' => "node/1/view"));
    $this->assertAliasExists(array('source' => "node/2"));
  }

  /**
   * Test the different update actions in pathauto_create_alias().
   */
  public function testUpdateActions() {
    $config = \Drupal::configFactory()->get('pathauto.settings');

    // Test PATHAUTO_UPDATE_ACTION_NO_NEW with unaliased node and 'insert'.
    $config->set('update_action', PATHAUTO_UPDATE_ACTION_NO_NEW);
    $config->save();
    $node = $this->drupalCreateNode(array('title' => 'First title'));
    $this->assertEntityAlias($node, 'content/first-title');

    // Default action is PATHAUTO_UPDATE_ACTION_DELETE.
    $config->set('update_action', PATHAUTO_UPDATE_ACTION_DELETE);
    $config->save();
    $node->setTitle('Second title');
    pathauto_node_update($node);
    $this->assertEntityAlias($node, 'content/second-title');
    $this->assertNoAliasExists(array('alias' => 'content/first-title'));

    // Test PATHAUTO_UPDATE_ACTION_LEAVE
    $config->set('update_action', PATHAUTO_UPDATE_ACTION_LEAVE);
    $config->save();
    $node->setTitle('Third title');
    pathauto_node_update($node);
    $this->assertEntityAlias($node, 'content/third-title');
    $this->assertAliasExists(array('source' => $node->getSystemPath(), 'alias' => 'content/second-title'));

    $config->set('update_action', PATHAUTO_UPDATE_ACTION_DELETE);
    $config->save();
    $node->setTitle('Fourth title');
    pathauto_node_update($node);
    $this->assertEntityAlias($node, 'content/fourth-title');
    $this->assertNoAliasExists(array('alias' => 'content/third-title'));
    // The older second alias is not deleted yet.
    $older_path = $this->assertAliasExists(array('source' => $node->getSystemPath(), 'alias' => 'content/second-title'));
    \Drupal::service('path.alias_storage')->delete($older_path);

    $config->set('pathauto_update_action', PATHAUTO_UPDATE_ACTION_NO_NEW);
    $config->save();
    $node->setTitle('Fifth title');
    pathauto_node_update($node);
    $this->assertEntityAlias($node, 'content/fourth-title');
    $this->assertNoAliasExists(array('alias' => 'content/fith-title'));

    // Test PATHAUTO_UPDATE_ACTION_NO_NEW with unaliased node and 'update'.
    $this->deleteAllAliases();
    pathauto_node_update($node);
    $this->assertEntityAlias($node, 'content/fifth-title');

    // Test PATHAUTO_UPDATE_ACTION_NO_NEW with unaliased node and 'bulkupdate'.
    $this->deleteAllAliases();
    $node->setTitle('Sixth title');
    pathauto_node_update_alias($node, 'bulkupdate');
    $this->assertEntityAlias($node, 'content/sixth-title');
  }

  /**
   * Test that pathauto_create_alias() will not create an alias for a pattern
   * that does not get any tokens replaced.
   */
  public function testNoTokensNoAlias() {
    $node = $this->drupalCreateNode(array('title' => ''));
    $this->assertNoEntityAliasExists($node);

    $node->setTitle('hello');
    pathauto_node_update($node);
    $this->assertEntityAlias($node, 'content/hello');
  }

  /**
   * Test the handling of path vs non-path tokens in pathauto_clean_token_values().
   */
  public function testPathTokens() {
    $config = \Drupal::configFactory()->get('pathauto.pattern');
    $config->set('taxonomy_term._default', '[term:parent:url:path]/[term:name]');
    $config->save();

    $vocab = $this->addVocabulary();

    $term1 = $this->addTerm($vocab, array('name' => 'Parent term'));
    $this->assertEntityAlias($term1, 'parent-term');

    $term2 = $this->addTerm($vocab, array('name' => 'Child term', 'parent' => $term1->id()));
    $this->assertEntityAlias($term2, 'parent-term/child-term');

    $this->saveEntityAlias($term1, 'My Crazy/Alias/');
    pathauto_taxonomy_term_update($term2);
    $this->assertEntityAlias($term2, 'My Crazy/Alias/child-term');
  }

  public function testEntityBundleRenamingDeleting() {
    $config = \Drupal::configFactory()->get('pathauto.pattern');

    // Create a vocabulary and test that it's pattern variable works.
    $vocab = $this->addVocabulary(array('machine_name' => 'old_name'));
    $config->set('taxonomy_term._default', 'base');
    $config->set("taxonomy_term.old_name", 'bundle');
    $config->save();

    $this->assertEntityPattern('taxonomy_term', 'old_name', Language::LANGCODE_NOT_SPECIFIED, 'bundle');

    // Rename the vocabulary's machine name, which should cause its pattern
    // variable to also be renamed.
    $vocab->vid = 'new_name';
    $vocab->save();
    $this->assertEntityPattern('taxonomy_term', 'new_name', Language::LANGCODE_NOT_SPECIFIED, 'bundle');
    $this->assertEntityPattern('taxonomy_term', 'old_name', Language::LANGCODE_NOT_SPECIFIED, 'base');

    // Delete the vocabulary, which should cause its pattern variable to also
    // be deleted.
    $vocab->delete();
    $this->assertEntityPattern('taxonomy_term', 'new_name', Language::LANGCODE_NOT_SPECIFIED, 'base');
  }

  function testNoExistingPathAliases() {
    $config = \Drupal::configFactory()->get('pathauto.settings');

    $config->set('node.page._default', '[node:title]');
    $config->set('punctuation_period', PATHAUTO_PUNCTUATION_DO_NOTHING);

    // Check that Pathauto does not create an alias of '/admin'.
    $node = $this->drupalCreateNode(array('title' => 'Admin', 'type' => 'page'));
    $this->assertEntityAlias($node, 'admin-0');

    // Check that Pathauto does not create an alias of '/modules'.
    $node->setTitle('Modules');
    $node->save();
    $this->assertEntityAlias($node, 'modules-0');

    // Check that Pathauto does not create an alias of '/index.php'.
    $node->setTitle('index.php');
    $node->save();
    $this->assertEntityAlias($node, 'index.php-0');

    // Check that a safe value gets an automatic alias. This is also a control
    // to ensure the above tests work properly.
    $node->setTitle('Safe value');
    $node->save();
    $this->assertEntityAlias($node, 'safe-value');
  }
}