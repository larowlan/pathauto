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
class PathautoPatternsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_patterns_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, array &$form_state) {
    $config = $this->configFactory->get('pathauto.pattern');

    $form = array();

    $all_settings = module_invoke_all('pathauto', 'settings');

    foreach ($all_settings as $settings) {
      $module = $settings->module;
      $patterndescr = $settings->patterndescr;
      $patterndefault = $settings->patterndefault;
      $groupheader = $settings->groupheader;

      $form[$module] = array(
        '#type' => 'fieldset',
        '#title' => $groupheader,
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      // Prompt for the default pattern for this module.
      $variable = $module . '._default';
      $form[$module][$variable] = array(
        '#type' => 'textfield',
        '#title' => $patterndescr,
        '#default_value' => $config->get($variable),
        '#size' => 65,
        '#maxlength' => 1280,
        '#element_validate' => array('token_element_validate'),
        '#after_build' => array('token_element_validate'),
        '#token_types' => array($settings->token_type),
        '#min_tokens' => 1,
        '#parents' => array($variable),
      );

      // If the module supports a set of specialized patterns, set
      // them up here.
      if (isset($settings->patternitems)) {
        foreach ($settings->patternitems as $itemname => $itemlabel) {
          $variable = $module . '_' . $itemname . '._default';
          $form[$module][$variable] = array(
            '#type' => 'textfield',
            '#title' => $itemlabel,
            // '#default_value' => variable_get($variable, ''),
            '#size' => 65,
            '#maxlength' => 1280,
            '#element_validate' => array('token_element_validate'),
            '#after_build' => array('token_element_validate'),
            '#token_types' => array($settings->token_type),
            '#min_tokens' => 1,
            '#parents' => array($variable),
          );
        }
      }

      // Display the user documentation of placeholders supported by
      // this module, as a description on the last pattern.
      $form[$module]['token_help'] = array(
        '#title' => t('Replacement patterns'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form[$module]['token_help']['help'] = array(
        '#theme' => 'token_tree',
        '#token_types' => array($settings->token_type),
      );
    }

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, array &$form_state) {


    parent::submitForm($form, $form_state);
  }

}
