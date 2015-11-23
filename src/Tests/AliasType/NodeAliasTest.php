<?php

/**
 * @file
 * Contains Drupal\pathauto\Tests\AliasType\NodeAliasTest
 */

namespace Drupal\pathauto\Tests\AliasType;

use Drupal\node\Entity\NodeType;
use Drupal\simpletest\KernelTestBase;

/**
 * Tests the node alias plugin.
 *
 * @group pathauto
 */
class NodeAliasTest extends KernelTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('pathauto','path', 'node', 'user', 'token');

  /**
   *
   */
  public function testNodeAlias() {
    /** @var \Drupal\pathauto\AliasTypeManager $manager */
    $manager = $this->container->get('plugin.manager.alias_type');

    NodeType::create(['type' => 'article', 'name' => 'Article'])->save();

    /** @var \Drupal\pathauto\AliasTypeInterface $node_type */
    $node_type = $manager->createInstance('node');

    $patterns = $node_type->getPatterns();
    $this->assertTrue((array_key_exists('article', $patterns)), "Article pattern exists.");
    $this->assertEqual($patterns['article'], 'Pattern for all Article paths', "Article pattern description matches.");

    $token_types = $node_type->getTokenTypes();
    $this->assertTrue(in_array('node', $token_types), "Node token type exists.");

    $label = $node_type->getLabel();
    $this->assertEqual($label, 'Content', "Plugin label matches.");

    $default_config = $node_type->defaultConfiguration();

    $this->assertTrue(array_key_exists('default', $default_config), "Default key exists.");
    $this->assertEqual($default_config['default'][0], '/content/[node:title]', "Default content pattern matches.");

  }

}
