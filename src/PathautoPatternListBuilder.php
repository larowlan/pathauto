<?php

/**
 * @file
 * Contains Drupal\pathauto\PathautoPatternListBuilder.
 */

namespace Drupal\pathauto;

use Drupal\Core\Config\Entity\DraggableListBuilder;
use Drupal\Core\Entity\EntityInterface;

/**
 * Provides a listing of Pathauto pattern entities.
 */
class PathautoPatternListBuilder extends DraggableListBuilder {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_pattern_list';
  }

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Pathauto pattern');
    $header['id'] = $this->t('Machine name');
    $header['type'] = $this->t('Pattern type');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var \Drupal\pathauto\PathautoPatternInterface $entity */
    $row['label'] = $entity->label();
    $row['id']['#markup'] = $entity->id();
    $row['type']['#markup'] = $entity->getAliasType()->getLabel();
    return $row + parent::buildRow($entity);
  }

}
