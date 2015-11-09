<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\AddContext.
 */

namespace Drupal\pathauto\Form;

use Drupal\Component\Serialization\Json;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\OpenModalDialogCommand;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

class AddContext extends FormBase {

  /**
   * @var string
   */
  protected $machine_name;

  /**
   * An array of property types that are eligible as relationships.
   *
   * @var array
   */
  protected $property_types = ['entity_reference', 'language'];

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'ctools_add_context_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $cached_values = $form_state->getTemporaryValue('wizard');
    $this->machine_name = $cached_values['id'];
    $form['items'] = array(
      '#type' => 'markup',
      '#prefix' => '<div id="configured-contexts">',
      '#suffix' => '</div>',
      '#theme' => 'table',
      '#header' => array($this->t('Context ID'), $this->t('Label'), $this->t('Data Type'), $this->t('Options')),
      '#rows' => $this->renderRows($cached_values),
      '#empty' => t('No contexts or relationships have been added.')
    );

    $form['relationships'] = [
      '#type' => 'select',
      '#title' => $this->t('Add a relationship'),
      '#options' => $this->getAvailableRelationships($cached_values),
    ];
    $form['add_relationship'] = [
      '#type' => 'submit',
      '#name' => 'add',
      '#value' => t('Add Relationship'),
      '#ajax' => [
        'callback' => [$this, 'addRelationship'],
        'event' => 'click',
      ],
      '#submit' => [
        'callback' => [$this, 'submitForm'],
      ]
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    if ($form_state->getTriggeringElement()['#name'] == 'add') {
      list(, $route_parameters) = $this->getRelationshipOperationsRouteInfo($this->machine_name, $form_state->getValue('relationships'));
      $form_state->setRedirect($this->getAddRoute(), $route_parameters);
    }
  }

  public function addRelationship(array &$form, FormStateInterface $form_state) {
    $relationship = $form_state->getValue('relationships');
    $content = \Drupal::formBuilder()->getForm($this->getContextClass(), $relationship, $this->getTempstoreId(), $this->machine_name);
    $content['#attached']['library'][] = 'core/drupal.dialog.ajax';
    list(, $route_parameters) = $this->getRelationshipOperationsRouteInfo($this->machine_name, $relationship);
    $content['submit']['#attached']['drupalSettings']['ajax'][$content['submit']['#id']]['url'] = $this->url($this->getAddRoute(), $route_parameters, ['query' => [FormBuilderInterface::AJAX_FORM_REQUEST => TRUE]]);
    $response = new AjaxResponse();
    $response->addCommand(new OpenModalDialogCommand($this->t('Configure Relationship'), $content, array('width' => '700')));
    return $response;
  }

  protected function getAvailableRelationships($cached_values) {
    /** @var \Drupal\ctools\TypedDataResolver $resolver */
    $resolver = \Drupal::service('ctools.typed_data.resolver');
    return $resolver->getTokensOfDataType($this->getContexts($cached_values), $this->property_types);
  }

  /**
   * @param $cached_values
   *
   * @return array
   */
  protected function renderRows($cached_values) {
    $contexts = array();
    foreach ($this->getContexts($cached_values) as $row => $context) {
      list($route_name, $route_parameters) = $this->getRelationshipOperationsRouteInfo($this->machine_name, $row);
      $build = array(
        '#type' => 'operations',
        '#links' => $this->getOperations($row, $route_name, $route_parameters),
      );
      $contexts[$row] = array(
        $row,
        $context->getContextDefinition()->getLabel(),
        $context->getContextDefinition()->getDataType(),
        'operations' => [
          'data' => $build,
        ],
      );
    }
    return $contexts;
  }

  protected function getOperations($row, $route_name_base, array $route_parameters = array()) {
    // Base contexts will not be a : separated and generated relationships should have 3 parts.
    if (count(explode(':', $row)) < 2) {
      return [];
    }
    $operations['edit'] = array(
      'title' => t('Edit'),
      'url' => new Url($route_name_base . '.edit', $route_parameters),
      'weight' => 10,
      'attributes' => array(
        'class' => ['use-ajax'],
        'data-dialog-type' => 'modal',
        'data-dialog-options' => Json::encode([
          'width' => 700,
        ]),
      ),
    );
    $route_parameters['id'] = $route_parameters['context'];
    $operations['delete'] = array(
      'title' => t('Delete'),
      'url' => new Url($route_name_base . '.delete', $route_parameters),
      'weight' => 100,
      'attributes' => array(
        'class' => array('use-ajax'),
        'data-dialog-type' => 'modal',
        'data-dialog-options' => Json::encode([
          'width' => 700,
        ]),
      ),
    );
    return $operations;
  }

  /**
   * Return a subclass of '\Drupal\ctools\Form\ContextConfigure'.
   *
   * The ConditionConfigure class is designed to be subclassed with custom
   * route information to control the modal/redirect needs of your use case.
   *
   * @return string
   */
  protected function getContextClass() {
    return ContextConfigure::class;
  }

  /**
   * The route to which condition 'add' actions should submit.
   *
   * @return string
   */
  protected function getAddRoute() {
    return 'pathauto.pattern.relationship.add';
  }

  /**
   * Provide the tempstore id for your specified use case.
   *
   * @return string
   */
  protected function getTempstoreId() {
    return 'pathauto.pattern';
  }

  /**
   * @param $cached_values
   *
   * @return \Drupal\Core\Plugin\Context\ContextInterface[]
   */
  protected function getContexts($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    return $pattern->getContexts();
  }

  protected function getRelationshipOperationsRouteInfo($machine_name, $row) {
    return ['pathauto.pattern.relationship', ['machine_name' => $machine_name, 'context' => $row]];
  }

}
