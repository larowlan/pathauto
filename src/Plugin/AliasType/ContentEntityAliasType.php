<?php

/**
 * @file
 * Contains Drupal\pathauto\Plugin\AliasType\NodeAliasType
 */

namespace Drupal\pathauto\Plugin\AliasType;
use Drupal\Core\Plugin\PluginBase;
use Drupal\pathauto\AliasTypeInterface;

/**
 * A pathauto alias type plugin for nodes.
 *
 * @AliasType(
 *   id = "node_alias_type",
 *   label = @Translation("Pathauto alias for nodes."),
 * )
 */
class NodeAliasType extends PluginBase implements AliasTypeInterface{


}
