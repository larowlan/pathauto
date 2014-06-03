<?php

/**
 * @file
 * Contains \Drupal\pathauto\Form\PathautoAdminDelete.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Form\FormBase;

/**
 * Configure file system settings for this site.
 */
class PathautoAdminDelete extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_admin_delete';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, array &$form_state) {

    $form = array();

    $form['delete'] = array(
      '#type' => 'fieldset',
      '#title' => t('Choose aliases to delete'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    // First we do the "all" case.
    $total_count = db_query('SELECT count(1) FROM {url_alias}')->fetchField();
    $form['delete']['all_aliases'] = array(
      '#type' => 'checkbox',
      '#title' => t('All aliases'),
      '#default_value' => FALSE,
      '#description' => t('Delete all aliases. Number of aliases which will be deleted: %count.', array('%count' => $total_count)),
    );

    // Next, iterate over an array of objects/alias types
    // which can be deleted and provide checkboxes.
    $args = func_get_args();
    // Remove $hook from the arguments.
    unset($args[0]);
    $objects = \Drupal::moduleHandler()->invokeAll('path_alias_types', $args);

    foreach ($objects as $internal_name => $label) {
      $count = db_query("SELECT count(1) FROM {url_alias} WHERE source LIKE :src", array(':src' => "$internal_name%"))->fetchField();
      $form['delete'][$internal_name] = array(
        '#type' => 'checkbox',
        '#title' => $label, // This label is sent through t() in the hard coded function where it is defined.
        '#default_value' => FALSE,
        '#description' => t('Delete aliases for all @label. Number of aliases which will be deleted: %count.', array('@label' => $label, '%count' => $count)),
      );
    }

    // Warn them and give a button that shows we mean business.
    $form['warning'] = array('#value' => '<p>' . t('<strong>Note:</strong> there is no confirmation. Be sure of your action before clicking the "Delete aliases now!" button.<br />You may want to make a backup of the database and/or the url_alias table prior to using this feature.') . '</p>');
    $form['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete aliases now!'),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, array &$form_state) {
    foreach ($form_state['values'] as $key => $value) {
      if ($value) {
        if ($key === 'all_aliases') {
          db_delete('url_alias')
            ->execute();
          drupal_set_message(t('All of your path aliases have been deleted.'));
        }
        $args = func_get_args();
        // Remove $hook from the arguments.
        unset($args[0]);
        $objects = \Drupal::moduleHandler()->invokeAll('path_alias_types', $args);
        if (array_key_exists($key, $objects)) {
          db_delete('url_alias')
            ->condition('source', db_like($key) . '%', 'LIKE')
            ->execute();
          drupal_set_message(t('All of your %type path aliases have been deleted.', array('%type' => $objects[$key])));
        }
      }
    }
    $form_state['redirect'] = 'admin/config/search/path/delete_bulk';
  }

}
