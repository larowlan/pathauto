<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\ContextDelete.
 */

namespace Drupal\pathauto\Form;


use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Form\ConfirmFormHelper;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\ctools\TypedDataResolver;
use Drupal\user\SharedTempStoreFactory;
use Symfony\Component\DependencyInjection\ContainerInterface;

class ContextDelete extends ConfirmFormBase {

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
   * @var string;
   */
  protected $id;

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

  public function getFormId() {
    return 'ctools_context_delete';
  }

  public function getQuestion($id = NULL, $cached_values = NULL) {
    $context = $this->getContexts($cached_values)[$id];
    return $this->t('Are you sure you want to delete the @label context?', [
      '@label' => $context->getContextDefinition()->getLabel(),
    ]);
  }

  public function getCancelUrl() {
    return new Url('entity.pathauto_pattern.edit_form', ['machine_name' => $this->machine_name, 'step' => 'contexts']);
  }

  public function buildForm(array $form, FormStateInterface $form_state, $id = NULL, $tempstore_id = NULL, $machine_name = NULL) {
    $this->tempstore_id = $tempstore_id;
    $this->machine_name = $machine_name;
    $this->id = $id;

    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);
    $form ['#title'] = $this->getQuestion($id, $cached_values);

    $form ['#attributes']['class'][] = 'confirmation';
    $form ['description'] = array('#markup' => $this->getDescription());
    $form [$this->getFormName()] = array('#type' => 'hidden', '#value' => 1);

    // By default, render the form using theme_confirm_form().
    if (!isset($form ['#theme'])) {
      $form ['#theme'] = 'confirm_form';
    }
    $form['actions'] = array('#type' => 'actions');
    $form['actions'] += $this->actions($form, $form_state);
    return $form;
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);;
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    $pattern->removeContext($this->id);
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    $form_state->setRedirectUrl($this->getCancelUrl());
  }

  protected function actions(array $form, FormStateInterface $form_state) {
    return array(
      'submit' => array(
        '#type' => 'submit',
        '#value' => $this->getConfirmText(),
        '#validate' => array(
          array($this, 'validate'),
        ),
        '#submit' => array(
          array($this, 'submitForm'),
        ),
      ),
      'cancel' => ConfirmFormHelper::buildCancelLink($this, $this->getRequest()),
    );
  }

  /**
   * @param $cached_values
   *
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  public function getContexts($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    return $pattern->getContexts();
  }

}
