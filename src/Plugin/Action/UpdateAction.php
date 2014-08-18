<?php
/**
 * @file
 * Contains: \Drupal\pathauto\Plugin\Action\UpdateAction
 */

namespace Drupal\pathauto\Plugin\Action;

use Drupal\Core\Action\ActionBase;

/**
 * Pathauto entity update action.
 *
 * @Action(
 *   id = "pathauto_update_alias",
 *   label = @Translation("Update URL-Alias of an entity"),
 * )
 */
class UpdateAction extends ActionBase {

  /**
   * {@inheritdoc}
   */
  public function execute($entity = NULL) {
    $entity->path = new \stdClass();
    $entity->path->pathauto = TRUE;
    \Drupal::service('pathauto.manager')->updateAlias($entity, 'bulkupdate', array('message' => TRUE));
  }
}
