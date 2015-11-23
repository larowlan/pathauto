<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\SelectionCriteriaForm.
 */

namespace Drupal\pathauto\Form;


use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Form\ManageConditions;

class SelectionCriteriaForm extends ManageConditions {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_pattern_selection_criteria_form';
  }

  /**
   * {@inheritdoc}
   */
  protected function getConditionClass() {
    return '\Drupal\pathauto\Form\CriteriaForm';
  }

  /**
   * {@inheritdoc}
   */
  protected function getAddRoute($cached_values) {
    return 'pathauto.pattern.condition.add';
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
  protected function getOperationsRouteInfo($cached_values, $machine_name, $row) {
    return ['pathauto.pattern.condition', ['machine_name' => $machine_name, 'condition' => $row]];
  }

  /**
   * {@inheritdoc}
   */
  protected function getConditions($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    $conditions = [];
    foreach ($pattern->getSelectionConditions() as $uuid => $configuration) {
      $conditions[$uuid] = $configuration->getConfiguration();
    }
    return $conditions;
  }

  /**
   * {@inheritdoc}
   */
  protected function getContexts($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    return $pattern->getContexts();
  }

}
