<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\CriteriaForm.
 */

namespace Drupal\pathauto\Form;


use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Form\ConditionConfigure;

class CriteriaForm extends ConditionConfigure {

  /**
   * {@inheritdoc}
   */
  protected function getParentRouteInfo() {
    return ['entity.pathauto_pattern.edit_form', ['machine_name' => $this->machine_name, 'step' => 'selection_criteria']];
  }

  /**
   * {@inheritdoc}
   */
  protected function getRouteInfo($condition) {
    return ['pathauto.pattern.condition.add', ['machine_name' => $this->machine_name, 'condition' => $condition]];
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
  protected function setConditions($cached_values, $conditions) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    // Set all the old conditions again since this is kind of indiscriminate.
    foreach ($conditions as $id => $configuration) {
      $pattern->getSelectionConditions()->setInstanceConfiguration($id, $conditions[$id]);
    }
    return $cached_values;
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