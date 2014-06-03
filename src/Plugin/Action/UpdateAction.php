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
 *   id = "pathauto_update_action",
 *   label = @Translation("Update URL-Alias of an entity"),
 *   type = "node"
 * )
 */
class UpdateAction extends ActionBase {

  /**
   * {@inheritdoc}
   */
  public function execute($entity = NULL) {
    switch ($entity->getEntityType()) {
      case 'node':
        pathauto_node_update_alias($entity, 'bulkupdate', array('message' => TRUE));
        break;

      case 'taxonomy_term':
        pathauto_taxonomy_term_update_alias($entity, 'bulkupdate', array('message' => TRUE));
        break;

      case 'user':
        pathauto_user_update_alias($entity, 'bulkupdate', array('message' => TRUE));
        break;
    }
  }
}
