<?php

/**
 * @file
 * Contains \Drupal\pathauto\Plugin\AliasType\NodeAliasType.
 */

namespace Drupal\pathauto\Plugin\pathauto\AliasType;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\pathauto\AliasTypeBatchUpdateInterface;

/**
 * A pathauto alias type plugin for content entities.
 *
 * @AliasType(
 *   id = "node",
 *   label = @Translation("Content"),
 *   types = {"node"},
 *   provider = "node",
 * )
 */
class NodeAliasType extends EntityAliasTypeBase implements AliasTypeBatchUpdateInterface, ContainerFactoryPluginInterface {

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('default' => array('/content/[node:title]')) + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  public function getSourcePrefix() {
    return 'node/';
  }

}
