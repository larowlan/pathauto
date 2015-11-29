<?php
/**
 * @file
 * Contains \Drupal\pathauto\Plugin\Deriver\EntityAliasTypeDeriver.
 */

namespace Drupal\pathauto\Plugin\Deriver;

use Drupal\Core\Entity\FieldableEntityInterface;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Plugin\Deriver\EntityDeriverBase;

/**
 * Deriver that exposes content entities as alias type plugins.
 */
class EntityAliasTypeDeriver extends EntityDeriverBase {

  /**
   * {@inheritdoc}
   */
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      // An entity type must have a canonical link template and support fields.
      if ($entity_type->hasLinkTemplate('canonical') && is_subclass_of($entity_type->getClass(), FieldableEntityInterface::class)) {
        $base_fields = $this->entityManager->getBaseFieldDefinitions($entity_type_id);
        if (!isset($base_fields['path'])) {
          // The entity type does not have a path field and is therefore not
          // supported.
          // @todo: Add a UI to enable that base field on any content entity.
          continue;
        }
        $this->derivatives[$entity_type_id] = $base_plugin_definition;
        $this->derivatives[$entity_type_id]['label'] = $entity_type->getLabel();
        $this->derivatives[$entity_type_id]['types'] = [$entity_type_id];
        $this->derivatives[$entity_type_id]['provider'] = $entity_type->getProvider();
        $this->derivatives[$entity_type_id]['context'] = [
          $entity_type_id => new ContextDefinition("entity:$entity_type_id", $this->t('@label being aliased', ['@label' => $entity_type->getLabel()]))
        ];
      }
    }
    return $this->derivatives;
  }

}
