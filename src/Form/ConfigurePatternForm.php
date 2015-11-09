<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\ConfigurePatternForm.
 */

namespace Drupal\pathauto\Form;


use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\pathauto\AliasTypeManager;
use Symfony\Component\DependencyInjection\ContainerInterface;

class ConfigurePatternForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_pattern_configure_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $pathauto_pattern \Drupal\pathauto\PathautoPatternInterface */
    $pathauto_pattern = $cached_values['pathauto_pattern'];
    $aliasType = $pathauto_pattern->getAliasType();
    $form = $aliasType->buildConfigurationForm($form, $form_state);
    $form['default']['#default_value'] = $pathauto_pattern->getPattern();
    // This is rough but generally seems right. I think Token is not nuanced
    // enough to handle this.
    $tokens = $form['default']['#token_types'];
    $contexts = $pathauto_pattern->getContexts();
    foreach ($contexts as $context_id => $context) {
      list($data_type, $entity_type) = explode(':', $context->getContextDefinition()->getDataType());
      if ($data_type == 'entity') {
        if ($entity_type == 'taxonomy_term') {
          $entity_type = 'term';
        }
        if (!in_array($entity_type, $tokens)) {
          $tokens[] = $entity_type;
        }
      }
    }
    $form['default']['#token_types'] = $tokens;

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $pathauto_pattern \Drupal\pathauto\PathautoPatternInterface */
    $pathauto_pattern = $cached_values['pathauto_pattern'];
    $pathauto_pattern->setPattern($form_state->getValue('default'));
  }

}
