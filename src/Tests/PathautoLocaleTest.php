<?php

/**
 * @file
 * Contains \Drupal\pathauto\Tests\PathautoLocaleTest.
 */

namespace Drupal\pathauto\Tests;

use Drupal\Core\Language\Language;

class PathautoLocaleTest extends PathautoFunctionalTestHelper {

  public static $modules = array('path', 'token', 'pathauto', 'taxonomy', 'views', 'locale');

  public static function getInfo() {
    return array(
      'name' => 'Pathauto localization tests',
      'description' => 'Test pathauto functionality with localization and translation.',
      'group' => 'Pathauto',
      // 'dependencies' => array('token'),
    );
  }

  /**
   * Test that when an English node is updated, its old English alias is
   * updated and its newer French alias is left intact.
   */
  function testLanguageAliases() {

    // Add predefined French language.
    $language = new Language(array('id' => 'fr'));
    language_save($language);

    $node = array(
      'title' => 'English node',
      'language' => 'en',
      'path' => array(array(
        'alias' => 'english-node',
        'pathauto' => FALSE,
      )),
    );
    $node = $this->drupalCreateNode($node);
    $english_alias = \Drupal::service('path.alias_storage')->load(array('alias' => 'english-node', 'langcode' => 'en'));
    $this->assertTrue($english_alias, 'Alias created with proper language.');

    // Also save a French alias that should not be left alone, even though
    // it is the newer alias.
    $this->saveEntityAlias($node, 'french-node', 'fr');

    // Add an alias with the soon-to-be generated alias, causing the upcoming
    // alias update to generate a unique alias with the '-0' suffix.
    $this->saveAlias('node/invalid', 'content/english-node', Language::LANGCODE_NOT_SPECIFIED);

    // Update the node, triggering a change in the English alias.
    $node->path->pathauto = TRUE;
    pathauto_entity_update($node);

    // Check that the new English alias replaced the old one.
    $this->assertEntityAlias($node, 'content/english-node-0', 'en');
    $this->assertEntityAlias($node, 'french-node', 'fr');
    $this->assertAliasExists(array('pid' => $english_alias['pid'], 'alias' => 'content/english-node-0'));
  }
}

