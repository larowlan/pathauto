<?php

namespace Drupal\pathauto\Tests\Pathauto;

use Drupal\Core\Language\Language;

class PathautoLocaleTestCase extends PathautoFunctionalTestHelper {
  public static function getInfo() {
    return array(
      'name' => 'Pathauto localization tests',
      'description' => 'Test pathauto functionality with localization and translation.',
      'group' => 'Pathauto',
      // 'dependencies' => array('token'),
    );
  }

  function setUp(array $modules = array()) {
    $modules[] = 'locale';
    $modules[] = 'translation';
    parent::setUp($modules, array('administer languages'));

    // Add predefined French language and reset the locale cache.
    require_once DRUPAL_ROOT . '/includes/locale.inc';

    locale_add_language('fr', NULL, NULL, Language::DIRECTION_LTR, '', 'fr');
    drupal_language_initialize();
  }

  /**
   * Test that when an English node is updated, its old English alias is
   * updated and its newer French alias is left intact.
   */
  function testLanguageAliases() {
    $node = array(
      'title' => 'English node',
      'language' => 'en',
      'body' => array('en' => array(array())),
      'path' => array(
        'alias' => 'english-node',
        'pathauto' => FALSE,
      ),
    );
    $node = $this->drupalCreateNode($node);
    $english_alias = path_load(array('alias' => 'english-node', 'language' => 'en'));
    $this->assertTrue($english_alias, 'Alias created with proper language.');

    // Also save a French alias that should not be left alone, even though
    // it is the newer alias.
    $this->saveEntityAlias('node', $node, 'french-node', 'fr');

    // Add an alias with the soon-to-be generated alias, causing the upcoming
    // alias update to generate a unique alias with the '-0' suffix.
    $this->saveAlias('node/invalid', 'content/english-node', Language::LANGCODE_NOT_SPECIFIED);

    // Update the node, triggering a change in the English alias.
    $node->path->pathauto = TRUE;
    pathauto_node_update($node);

    // Check that the new English alias replaced the old one.
    $this->assertEntityAlias('node', $node, 'content/english-node-0', 'en');
    $this->assertEntityAlias('node', $node, 'french-node', 'fr');
    $this->assertAliasExists(array('pid' => $english_alias['pid'], 'alias' => 'content/english-node-0'));
  }
}

