<?php

/**
 * @file
 * Contains Drupal\pathauto\Plugin\AliasType\AliasTypeBase
 */

namespace Drupal\pathauto\Plugin\AliasType;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\PluginBase;
use Drupal\pathauto\AliasTypeInterface;

/**
 * A base class for Alias Type plugins.
 */
abstract class AliasTypeBase  extends PluginBase implements AliasTypeInterface {


  /**
   * {@inheritdoc}
   */
  public function getConfiguration() {
    return $this->configuration;
  }

  /**
   * {@inheritdoc}
   */
  public function setConfiguration(array $configuration) {
    $this->configuration = $configuration;
  }

  /**
   * {@inheritdoc}
   */
  public function setConfigurationValue($key, $value) {
    $this->configuration[$key] = $value;
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array();
  }

  /**
   * {@inheritdoc}
   */
  public function calculateDependencies() {
    return array();
  }

  /**
   * {@inheritdoc}
   */
  public function validateConfigurationForm(array &$form, FormStateInterface $form_state) {
    return $form_state;
  }

  /**
   * Get the label.
   *
   * @return string
   *   The label.
   */
  public abstract function getLabel();

  /**
   * Get the pattern description.
   *
   * @return string
   *   The pattern description.
   */
  public abstract function getPatternDescription();

  /**
   * Get the patterns.
   *
   * @return string[]
   *   The array of patterns.
   */
  public abstract function getPatterns();

  /**
   * Get the token type.
   *
   * @return string
   *   The token type.
   */
  public abstract function getTokenType();

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    $plugin_id = $this->getPluginId();

    $form[$plugin_id] = array(
      '#type' => 'fieldset',
      '#title' => $this->getLabel(),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
    );

    // Prompt for the default pattern for this module.
    $key = '_default';

    $form[$plugin_id][$key] = array(
      '#type' => 'textfield',
      '#title' => $this->getPatternDescription(),
      '#default_value' => $this->configuration['patternitems'],
      '#size' => 65,
      '#maxlength' => 1280,
      '#element_validate' => array('token_element_validate'),
      '#after_build' => array('token_element_validate'),
      '#token_types' => array($this->getTokenType()),
      '#min_tokens' => 1,
    );

    // If the module supports a set of specialized patterns, set
    // them up here.
    $patterns = $this->getPatterns();
    foreach ($patterns as $itemname => $itemlabel) {
      $key = '_default';

      $form[$plugin_id][$itemname][$key] = array(
        '#type' => 'textfield',
        '#title' => $itemlabel,
        '#default_value' => $this->configuration[$plugin_id . '.' . $itemname . '.' . $key],
        '#size' => 65,
        '#maxlength' => 1280,
        '#element_validate' => array('token_element_validate'),
        '#after_build' => array('token_element_validate'),
        '#token_types' => array($this->getTokenType()),
        '#min_tokens' => 1,
      );
    }

    // Display the user documentation of placeholders supported by
    // this module, as a description on the last pattern.
    $form[$plugin_id]['token_help'] = array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form[$plugin_id]['token_help']['help'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array($this->getTokenType()),
    );
  }

}
