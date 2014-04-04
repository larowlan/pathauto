<?php

/**
 * @file
 * Contains \Drupal\pathauto\Form\MaillogSettingsForm.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Form\ConfigFormBase;

/**
 * Configure file system settings for this site.
 */
class PathautoBulkUpdateForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_bulk_update_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, array &$form_state) {

    $form = array();

    $form['#update_callbacks'] = array();

    $form['update'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Select the types of un-aliased paths for which to generate URL aliases'),
      '#options' => array(),
      '#default_value' => array(),
    );

    $pathauto_settings = module_invoke_all('pathauto', 'settings');

    foreach ($pathauto_settings as $settings) {
      if (!empty($settings->batch_update_callback)) {
        $form['#update_callbacks'][$settings->batch_update_callback] = $settings;
        $form['update']['#options'][$settings->batch_update_callback] = $settings->groupheader;
      }
    }

    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
    );

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, array &$form_state) {

    parent::submitForm($form, $form_state);
  }

}
