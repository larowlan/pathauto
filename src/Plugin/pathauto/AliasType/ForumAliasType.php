<?php

/**
 * @file
 * Contains \Drupal\pathauto\Plugin\AliasType\ForumAliasType.
 */

namespace Drupal\pathauto\Plugin\pathauto\AliasType;

use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\pathauto\AliasTypeBatchUpdateInterface;

/**
 * A pathauto alias type plugin for forum terms.
 *
 * @AliasType(
 *   id = "forum",
 *   label = @Translation("Forum"),
 *   types = {"term"},
 *   provider = "forum",
 * )
 */
class ForumAliasType extends EntityAliasTypeBase implements AliasTypeBatchUpdateInterface, ContainerFactoryPluginInterface {

  /**
   * {@inheritdoc}
   */
  public function getPatternDescription() {
    return $this->t('Pattern for forums and forum containers');
  }

  /**
   * {@inheritdoc}
   */
  public function getPatterns() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('default' => array('/[term:vocabulary]/[term:name]')) + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  protected function getEntityTypeId() {
    return 'taxonomy_term';
  }

  /**
   * {@inheritdoc}
   */
  public function getSourcePrefix() {
    return '/forum/';
  }

}
