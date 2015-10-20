<?php

/**
 * @file
 * Contains \Drupal\pathauto\Plugin\AliasType\UserAliasType.
 */

namespace Drupal\pathauto\Plugin\pathauto\AliasType;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\pathauto\AliasTypeBatchUpdateInterface;

/**
 * A pathauto alias type plugin for user entities.
 *
 * @AliasType(
 *   id = "user",
 *   label = @Translation("User"),
 *   types = {"user"},
 *   provider = "user",
 * )
 */
class UserAliasType extends EntityAliasTypeBase implements AliasTypeBatchUpdateInterface, ContainerFactoryPluginInterface {

  /**
   * {@inheritdoc}
   */
  public function getPatternDescription() {
    return $this->t('Pattern for user account page paths');
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('default' => array('/users/[user:name]')) + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  public function getSourcePrefix() {
    return '/user/';
  }

}
