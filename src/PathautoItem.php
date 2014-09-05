<?php

/**
 * @file
 * Contains \Drupal\pathauto\PathautoItem.
 */

namespace Drupal\pathauto;

use Drupal\path\Plugin\Field\FieldType\PathItem;

/**
 * Extends the default PathItem implementation to generate aliases.
 */
class PathautoItem extends PathItem {

  /**
   * {@inheritdoc}
   */
  public function insert() {
    if (isset($this->pathauto) && empty($this->pathauto)) {
      parent::insert();
    }
    else {
      \Drupal::service('pathauto.manager')->updateAlias($this->getEntity(), 'insert');
    }
  }

  /**
   * {@inheritdoc}
   */
  public function update() {
    if (isset($this->pathauto) && empty($this->pathauto)) {
      parent::update();
    }
    else {
      \Drupal::service('pathauto.manager')->updateAlias($this->getEntity(), 'update');
    }
  }

  /**
   * {@inheritdoc}
   */
  public function delete() {
    pathauto_entity_path_delete_all($this->getEntity());
  }

} 
