<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\ContextConfigure.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Form\RelationshipConfigure;

class ContextConfigure extends RelationshipConfigure {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_relationship_configure';
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    if (!$pattern->hasContext($form_state->getValue('id'))) {
      /** @var \Drupal\Core\Plugin\Context\ContextInterface $context */
      $context = $form_state->getValue('context_object');
      $definition = $context->getContextDefinition();
      $new_definition = new ContextDefinition($definition->getDataType(), $form_state->getValue('label'), $definition->isRequired(), $definition->isMultiple(), $definition->getDescription(), $definition->getDefaultValue());
      $new_context = new Context($new_definition, $context->hasContextValue() ? $context->getContextValue() : NULL);
      $pattern->addContext($form_state->getValue('id'), $new_context);
    }
    else {
      $context = $pattern->getContext($form_state->getValue('id'));
      $definition = $context->getContextDefinition();
      $new_definition = new ContextDefinition($definition->getDataType(), $form_state->getValue('label'), $definition->isRequired(), $definition->isMultiple(), $definition->getDescription(), $definition->getDefaultValue());
      $new_context = new Context($new_definition, $context->hasContextValue() ? $context->getContextValue() : NULL);
      $pattern->replaceContext($form_state->getValue('id'), $new_context);
    }
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    parent::submitForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  protected function getParentRouteInfo($cached_values) {
    return ['entity.pathauto_pattern.edit_form', ['machine_name' => $this->machine_name, 'step' => 'contexts']];
  }

  /**
   * {@inheritdoc}
   */
  protected function setContexts($cached_values, $contexts) {}

  /**
   * {@inheritdoc}
   */
  protected function getContexts($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    return $pattern->getContexts();
  }

}