<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\ContextConfigure.
 */

namespace Drupal\pathauto\Form;

use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\CloseModalDialogCommand;
use Drupal\Core\Ajax\RedirectCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\Context\Context;
use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\TypedDataResolver;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

class ContextConfigure extends FormBase {

  /**
   * @var \Drupal\user\SharedTempStoreFactory
   */
  protected $tempstore;

  /**
   * @var \Drupal\ctools\TypedDataResolver
   */
  protected $resolver;

  /**
   * @var string
   */
  protected $tempstore_id;

  /**
   * @var string;
   */
  protected $machine_name;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('user.shared_tempstore'), $container->get('ctools.typed_data.resolver'));
  }

  public function __construct(SharedTempStoreFactory $tempstore, TypedDataResolver $resolver) {
    $this->tempstore = $tempstore;
    $this->resolver = $resolver;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_context_configure';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state, $context = NULL, $tempstore_id = NULL, $machine_name = NULL) {
    $this->tempstore_id = $tempstore_id;
    $this->machine_name = $machine_name;
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);

    /** @var \Drupal\Core\Plugin\Context\ContextInterface[] $contexts */
    $contexts = $this->getContexts($cached_values);
    $context_object = $this->resolver->convertTokenToContext($context, $contexts);
    $form['id'] = [
      '#type' => 'value',
      '#value' => $context
    ];
    $form['context_object'] = [
      '#type' => 'value',
      '#value' => $context_object,
    ];
    $form['context_data'] = [
      '#type' => 'item',
      '#title' => $this->resolver->getLabelByToken($context, $contexts),
      '#markup' => $context_object->getContextDefinition()->getDataType(),
    ];
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Context label'),
      '#default_value' => !empty($contexts[$context]) ? $contexts[$context]->getContextDefinition()->getLabel() : '',
      '#required' => TRUE,
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Save'),
      '#ajax' => [
        'callback' => [$this, 'ajaxSave'],
      ]
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    if (!$pattern->hasContext($form_state->getValue('id'))) {
      /** @var \Drupal\Core\Plugin\Context\ContextInterface $context */
      $context = $form_state->getValue('context_object');
      $definition = $context->getContextDefinition();
      $new_definition = new ContextDefinition($definition->getDataType(), $form_state->getValue('label'), $definition->isRequired(), $definition->isMultiple(), $definition->getDescription(), $definition->getDefaultValue());
      $new_context = new Context($new_definition, $context->hasContextValue() ? $context->getContextValue() : NULL);
      $pattern->addContext($form_state->getValue('id'), $new_context);
    }
    else {
      $context = $pattern->getContext($form_state->getValue('id'));
      $definition = $context->getContextDefinition();
      $new_definition = new ContextDefinition($definition->getDataType(), $form_state->getValue('label'), $definition->isRequired(), $definition->isMultiple(), $definition->getDescription(), $definition->getDefaultValue());
      $new_context = new Context($new_definition, $context->hasContextValue() ? $context->getContextValue() : NULL);
      $pattern->replaceContext($form_state->getValue('id'), $new_context);
    }
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    list($route_name, $route_options) = $this->getParentRouteInfo();
    $form_state->setRedirect($route_name, $route_options);
  }

  public function ajaxSave(array &$form, FormStateInterface $form_state) {
    $response = new AjaxResponse();
    list($route_name, $route_parameters) = $this->getParentRouteInfo();
    $response->addCommand(new RedirectCommand($this->url($route_name, $route_parameters)));
    $response->addCommand(new CloseModalDialogCommand());
    return $response;
  }

  /**
   * Document the route name and parameters for redirect after submission.
   *
   * @return array
   *   In the format of
   *   return ['route.name', ['machine_name' => $this->machine_name, 'step' => 'step_name']];
   */
  protected function getParentRouteInfo() {
    return ['entity.pathauto_pattern.edit_form', ['machine_name' => $this->machine_name, 'step' => 'contexts']];
  }

  /**
   * @param $context
   *
   * @return array
   *   In the format of
   *   return ['route.name', ['machine_name' => $this->machine_name, 'context' => $context]];
   */
  protected function getRouteInfo($context) {
    return ['pathauto.pattern.relationship.add', ['machine_name' => $this->machine_name, 'context' => $context]];
  }

  /**
   * Custom logic for setting the conditions array in cached_values.
   *
   * @param $cached_values
   *
   * @param $contexts
   *   The conditions to set within the cached values.
   *
   * @return mixed
   *   Return the $cached_values
   */
  protected function setContexts($cached_values, $contexts) {}

  /**
   * Custom logic for retrieving the contexts array from cached_values.
   *
   * @param $cached_values
   *
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  protected function getContexts($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    return $pattern->getContexts();
  }

}