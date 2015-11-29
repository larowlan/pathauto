<?php

/**
 * @file
 * Contains \Drupal\pathauto\Plugin\AliasType\EntityAliasTypeBase.
 */

namespace Drupal\pathauto\Plugin\pathauto\AliasType;

use Drupal\Component\Utility\NestedArray;
use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Entity\FieldableEntityInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Language\LanguageManagerInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Plugin\ContextAwarePluginBase;
use Drupal\pathauto\AliasTypeBatchUpdateInterface;
use Drupal\pathauto\AliasTypeInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * A pathauto alias type plugin for entities with canonical links.
 *
 * @AliasType(
 *   id = "canonical_entities",
 *   deriver = "\Drupal\pathauto\Plugin\Deriver\EntityAliasTypeDeriver"
 * )
 */
class EntityAliasTypeBase extends ContextAwarePluginBase implements AliasTypeInterface, AliasTypeBatchUpdateInterface, ContainerFactoryPluginInterface {

  /**
   * The module handler service.
   *
   * @var \Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $moduleHandler;

  /**
   * The language manager service.
   *
   * @var \Drupal\Core\Language\LanguageManagerInterface
   */
  protected $languageManager;

  /**
   * The entity manager service.
   *
   * @var \Drupal\Core\Entity\EntityManagerInterface
   */
  protected $entityManager;

  /**
   * The path prefix for this entity type.
   *
   * @var string
   */
  protected $prefix;

  /**
   * Constructs a EntityAliasTypeBase instance.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler service.
   * @param \Drupal\Core\Language\LanguageManagerInterface $language_manager
   *   The language manager service.
   * @param \Drupal\Core\Entity\EntityManagerInterface $entity_manager
   *   The entity manager service.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, ModuleHandlerInterface $module_handler, LanguageManagerInterface $language_manager, EntityManagerInterface $entity_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->moduleHandler = $module_handler;
    $this->languageManager = $language_manager;
    $this->entityManager = $entity_manager;
    $this->setConfiguration($configuration);
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('module_handler'),
      $container->get('language_manager'),
      $container->get('entity.manager')
    );
  }

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
    $this->configuration = NestedArray::mergeDeep(
      $this->defaultConfiguration(),
      $configuration
    );
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
  public function getLabel() {
    $definition = $this->getPluginDefinition();
    // Cast the admin label to a string since it is an object.
    // @see \Drupal\Core\StringTranslation\TranslationWrapper
    return (string) $definition['label'];
  }

  /**
   * {@inheritdoc}
   */
  public function getTokenTypes() {
    $definition = $this->getPluginDefinition();
    // For some reason, we didn't unify token keys with entity types...
    foreach ($definition['types'] as $key => $type) {
      if ($type == 'taxonomy_term') {
        $definition['types'][$key] = 'term';
      }
    }
    return $definition['types'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    $form = array(
      '#type' => 'details',
      '#title' => $this->getLabel(),
    );

    $form['default'] = array(
      '#type' => 'textfield',
      '#title' => 'Path pattern',
      '#default_value' => !empty($this->configuration['default']) ? $this->configuration['default'] : '',
      '#size' => 65,
      '#maxlength' => 1280,
      '#element_validate' => array('token_element_validate'),
      '#after_build' => array('token_element_validate'),
      '#token_types' => $this->getTokenTypes(),
      '#min_tokens' => 1,
    );

    // Show the token help relevant to this pattern type.
    $form['token_help'] = array(
      '#theme' => 'token_tree',
      '#token_types' => $this->getTokenTypes(),
      '#dialog' => TRUE,
    );
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function batchUpdate(&$context) {
    if (!isset($context['sandbox']['current'])) {
      $context['sandbox']['count'] = 0;
      $context['sandbox']['current'] = 0;
    }

    $entity_type = $this->entityManager->getDefinition($this->getEntityTypeId());
    $id_key = $entity_type->getKey('id');

    $query = db_select($entity_type->get('base_table'), 'base_table');
    $query->leftJoin('url_alias', 'ua', "CONCAT('" . $this->getSourcePrefix() . "' , base_table.$id_key) = ua.source");
    $query->addField('base_table', $id_key, 'id');
    $query->isNull('ua.source');
    $query->condition('base_table.' . $id_key, $context['sandbox']['current'], '>');
    $query->orderBy('base_table.' . $id_key);
    $query->addTag('pathauto_bulk_update');
    $query->addMetaData('entity', $this->getEntityTypeId());

    // Get the total amount of items to process.
    if (!isset($context['sandbox']['total'])) {
      $context['sandbox']['total'] = $query->countQuery()->execute()->fetchField();

      // If there are no entities to update, then stop immediately.
      if (!$context['sandbox']['total']) {
        $context['finished'] = 1;
        return;
      }
    }

    $query->range(0, 25);
    $ids = $query->execute()->fetchCol();

    $this->bulkUpdate($ids);
    $context['sandbox']['count'] += count($ids);
    $context['sandbox']['current'] = max($ids);
    $context['message'] = t('Updated alias for %label @id.', array('%label' => $entity_type->getLabel(), '@id' => end($ids)));

    if ($context['sandbox']['count'] != $context['sandbox']['total']) {
      $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
    }
  }

  /**
   * Returns the entity type ID.
   *
   * @return string
   *   The entity type ID.
   */
  protected function getEntityTypeId() {
    return $this->getDerivativeId();
  }

  /**
   * Update the URL aliases for multiple entities.
   *
   * @param array $ids
   *   An array of entity IDs IDs.
   * @param array $options
   *   An optional array of additional options.
   */
  protected function bulkUpdate(array $ids, array $options = array()) {
    $options += array('message' => FALSE);

    $entities = $this->entityManager->getStorage($this->getEntityTypeId())->loadMultiple($ids);
    foreach ($entities as $entity) {
      \Drupal::service('pathauto.generator')->updateEntityAlias($entity, 'bulkupdate', $options);
    }

    if (!empty($options['message'])) {
      drupal_set_message(\Drupal::translation()->formatPlural(count($ids), 'Updated 1 %label URL alias.', 'Updated @count %label URL aliases.'), array('%label' => $this->getLabel()));
    }
  }

  /**
   * {@inheritdoc}
   */
  public function calculateDependencies() {
  }

  /**
   * {@inheritdoc}
   */
  public function validateConfigurationForm(array &$form, FormStateInterface $form_state) {
  }

  /**
   * {@inheritdoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
  }

  /**
   * {@inheritdoc}
   */
  public function applies($object) {
    return $object instanceof FieldableEntityInterface && $object->getEntityTypeId() == $this->getEntityTypeId();
  }

  /**
   * {@inheritdoc}
   */
  public function getSourcePrefix() {
    if (empty($this->prefix)) {
      $entity_type = $this->entityManager->getDefinition($this->getEntityTypeId());
      $path = $entity_type->getLinkTemplate('canonical');
      $this->prefix = substr($path, 0, strpos($path, '{'));
    }
    return $this->prefix;
  }

}
