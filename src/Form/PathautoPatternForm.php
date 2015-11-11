<?php

/**
 * @file
 * Contains Drupal\pathauto\Form\PathautoPatternForm.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\pathauto\AliasTypeManager;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class PathautoPatternForm.
 *
 * @package Drupal\pathauto\Form
 */
class PathautoPatternForm extends FormBase {

  /**
   * @var \Drupal\pathauto\AliasTypeManager
   */
  protected $manager;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('plugin.manager.alias_type'));
  }

  /**
   * @param \Drupal\pathauto\AliasTypeManager $manager
   */
  function __construct(AliasTypeManager $manager) {
    $this->manager = $manager;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_pattern_general_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $pathauto_pattern \Drupal\pathauto\PathautoPatternInterface */
    $pathauto_pattern = $cached_values['pathauto_pattern'];
    $options = [];
    foreach ($this->manager->getDefinitions() as $plugin_id => $plugin_definition) {
      $options[$plugin_id] = $plugin_definition['label'];
    }
    $form['type'] = [
      '#type' => 'select',
      '#title' => $this->t('Pattern type'),
      '#default_value' => $pathauto_pattern->getType(),
      '#options' => $options,
      '#required' => TRUE,
    ];

    return $form;
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    /** @var $pathauto_pattern \Drupal\pathauto\PathautoPatternInterface */
    $pathauto_pattern = $cached_values['pathauto_pattern'];
    $pathauto_pattern->set('label', $form_state->getValue('label'));
    $pathauto_pattern->set('id', $form_state->getValue('id'));
    $pathauto_pattern->set('type', $form_state->getValue('type'));
  }


}
