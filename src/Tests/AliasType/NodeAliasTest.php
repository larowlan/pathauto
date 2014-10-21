<?php

/**
 * @file
 * Contains Drupal\pathauto\Tests\AliasType\NodeAliasTest
 */

namespace Drupal\pathauto\Tests\AliasType;

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
  public static $modules = array('pathauto','path');

  /**
   *
   */
  public function testNodeAlias() {
    /** @var \Drupal\pathauto\AliasTypeManager $manager */
    $manager = $this->container->get('plugin.manager.alias_type');
    $definitions = $manager->getDefinitions();
    $node_type = $manager->createInstance('node');

  }

}
