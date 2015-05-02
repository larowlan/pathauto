<?php

/**
 * @file
 * Contains \Drupal\pathauto\Form\MaillogSettingsForm.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\pathauto\AliasTypeManager;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Configure file system settings for this site.
 */
class PathautoPatternsForm extends ConfigFormBase {

  /**
   * The alias type manager.
   *
   * @var \Drupal\pathauto\AliasTypeManager
   */
  protected $aliasTypeManager;

  /**
   * Constructs a PathautoPatternsForm object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The factory for configuration objects.
   */
  public function __construct(ConfigFactoryInterface $config_factory, AliasTypeManager $alias_type_manager) {
    parent::__construct($config_factory);
    $this->aliasTypeManager = $alias_type_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory'),
      $container->get('plugin.manager.alias_type')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_patterns_form';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['pathauto.pattern'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $definitions = $this->aliasTypeManager->getDefinitions();

    $config = $this->config('pathauto.pattern');

    foreach ($definitions as $id => $definition) {
      /** @var \Drupal\pathauto\AliasTypeInterface $alias_type */
      $alias_type = $this->aliasTypeManager->createInstance($id);

      $form[$id] = array(
        '#type' => 'fieldset',
        '#title' => $alias_type->getLabel(),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
      );

      // Prompt for the default pattern for this module.
      $key = 'default';

      $form[$id][$key] = array(
        '#type' => 'textfield',
        '#title' => $alias_type->getPatternDescription(),
        '#default_value' => $config->get('patterns.' . $id . '.' . $key),
        '#size' => 65,
        '#maxlength' => 1280,
        '#element_validate' => array('token_element_validate'),
        '#after_build' => array('token_element_validate'),
        '#token_types' => $alias_type->getTokenTypes(),
        '#min_tokens' => 1,
      );

      // If the module supports a set of specialized patterns, set
      // them up here.
      if ($alias_type->getPatterns()) {
        foreach ($alias_type->getPatterns() as $itemname => $itemlabel) {
          $key = 'default';

          $form[$id]['bundles'][$itemname][$key] = array(
            '#type' => 'textfield',
            '#title' => $itemlabel,
            '#default_value' => $config->get('patterns.'. $id . '.bundles.' . $itemname . '.' . $key),
            '#size' => 65,
            '#maxlength' => 1280,
            '#element_validate' => array('token_element_validate'),
            '#after_build' => array('token_element_validate'),
            '#token_types' => $alias_type->getTokenTypes(),
            '#min_tokens' => 1,
          );
        }
      }

      // Display the user documentation of placeholders supported by
      // this module, as a description on the last pattern.
      $form[$id]['token_help'] = array(
        '#title' => t('Replacement patterns'),
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form[$id]['token_help']['help'] = array(
        '#theme' => 'token_tree',
        '#token_types' => $alias_type->getTokenTypes(),
      );
    }

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

    $config = $this->config('pathauto.pattern');

    $definitions = $this->aliasTypeManager->getDefinitions();

    foreach ($definitions as $id => $definition) {
      $config->set('patterns.' . $id, $form_state->getValue($id));
    }

    $config->save();

    parent::submitForm($form, $form_state);
  }

}
