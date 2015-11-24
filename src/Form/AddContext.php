<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\AddContext.
 */

namespace Drupal\pathauto\Form;

use Drupal\ctools\Form\ManageContext;

class AddContext extends ManageContext {

  /**
   * An array of property types that are eligible as relationships.
   *
   * @var array
   */
  protected $property_types = ['entity_reference', 'language'];

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_add_context_form';
  }

  /**
   * {@inheritdoc}
   */
  protected function getContextClass($cached_values) {
    return ContextConfigure::class;
  }

  /**
   * {@inheritdoc}
   */
  protected function getAddRoute($cached_values) {
    return 'pathauto.pattern.relationship.add';
  }

  /**
   * {@inheritdoc}
   */
  protected function getTempstoreId() {
    return 'pathauto.pattern';
  }

  /**
   * {@inheritdoc}
   */
  protected function getContexts($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    return $pattern->getContexts();
  }

  /**
   * {@inheritdoc}
   */
  protected function getRelationshipOperationsRouteInfo($cached_values, $machine_name, $row) {
    return ['pathauto.pattern.relationship', ['machine_name' => $machine_name, 'context' => $row]];
  }

}
