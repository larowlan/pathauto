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
