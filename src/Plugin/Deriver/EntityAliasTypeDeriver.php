<?php
/**
 * @file
 * Contains \Drupal\pathauto\Plugin\Deriver\EntityAliasTypeDeriver.
 */

namespace Drupal\pathauto\Plugin\Deriver;

use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Plugin\Deriver\EntityDeriverBase;

class EntityAliasTypeDeriver extends EntityDeriverBase {
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      if ($entity_type->hasLinkTemplate('canonical')) {
        $this->derivatives[$entity_type_id] = $base_plugin_definition;
        $this->derivatives[$entity_type_id]['label'] = $this->t('@label', ['@label' => $entity_type->getLabel()]);
        $this->derivatives[$entity_type_id]['types'] = [$entity_type_id];
        $this->derivatives[$entity_type_id]['provider'] = $entity_type->getProvider();
        $this->derivatives[$entity_type_id]['context'] = [
          "$entity_type_id" => new ContextDefinition("entity:$entity_type_id", $this->t('@label being aliased', ['@label' => $entity_type->getLabel()]))
        ];
      }
    }
    return $this->derivatives;
  }
}