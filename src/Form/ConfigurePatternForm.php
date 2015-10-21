<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\ConfigurePatternForm.
 */

namespace Drupal\pathauto\Form;


use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

class ConfigurePatternForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static();
  }

  /**
   * {@inheritdoc}
   */
  function __construct() {

  }

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
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

  }

}
