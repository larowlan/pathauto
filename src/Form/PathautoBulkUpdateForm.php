<?php

/**
 * @file
 * Contains \Drupal\pathauto\Form\PathautoBulkUpdateForm.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Form\FormBase;

/**
 * Configure file system settings for this site.
 */
class PathautoBulkUpdateForm extends FormBase {

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

    $pathauto_settings = \Drupal::moduleHandler()->invokeAll('pathauto', array('settings'));

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

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, array &$form_state) {
    $batch = array(
      'title' => t('Bulk updating URL aliases'),
      'operations' => array(
        array('Drupal\pathauto\Form\PathautoBulkUpdateForm::batchStart', array()),
      ),
      'finished' => 'Drupal\pathauto\Form\PathautoBulkUpdateForm::batchFinished',
    );

    foreach ($form_state['values']['update'] as $callback) {
      if (!empty($callback)) {
        $settings = $form['#update_callbacks'][$callback];
        if (!empty($settings->batch_file)) {
          $batch['operations'][] = array('Drupal\pathauto\Form\PathautoBulkUpdateForm::batchProcess', array($callback, $settings));
        }
        else {
          $batch['operations'][] = array($callback, array());
        }
      }
    }

    batch_set($batch);
  }

  /**
   * Batch callback; count the current number of URL aliases for comparison later.
   */
  public static function batchStart(&$context) {
    $context['results']['count_before'] = db_select('url_alias')->countQuery()->execute()->fetchField();
  }

  /**
   * Common batch processing callback for all operations.
   *
   * Required to load our include the proper batch file.
   */
  public static function batchProcess($callback, $settings, &$context) {
    if (!empty($settings->batch_file)) {
      require_once DRUPAL_ROOT . '/' . $settings->batch_file;
    }
    return $callback($context);
  }

  /**
   * Batch finished callback.
   */
  public static function batchFinished($success, $results, $operations) {
    if ($success) {
      // Count the current number of URL aliases after the batch is completed
      // and compare to the count before the batch started.
      $results['count_after'] = db_select('url_alias')->countQuery()->execute()->fetchField();
      $results['count_changed'] = max($results['count_after'] - $results['count_before'], 0);
      if ($results['count_changed']) {
        drupal_set_message(\Drupal::translation()->formatPlural($results['count_changed'], 'Generated 1 URL alias.', 'Generated @count URL aliases.'));
      }
      else {
        drupal_set_message(t('No new URL aliases to generate.'));
      }
    }
    else {
      $error_operation = reset($operations);
      drupal_set_message(t('An error occurred while processing @operation with arguments : @args', array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))));
    }
  }

}
