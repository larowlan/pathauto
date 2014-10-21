<?php

/**
 * @file
 * Contains Drupal\pathauto\Plugin\AliasType\NodeAliasType
 */

namespace Drupal\pathauto\Plugin\pathauto\AliasType;

use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Language\LanguageInterface;
use Drupal\Core\Language\LanguageManagerInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * A pathauto alias type plugin for content entities.
 *
 * @AliasType(
 *   id = "node",
 *   label = @Translation("Content"),
 *   types = {"node"},
 *   provider = "node",
 * )
 */
class NodeAliasType extends AliasTypeBase implements ContainerFactoryPluginInterface {

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
   * Constructs a NodeAliasType instance.
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
  public function getPatternDescription() {
    $this->t('Default path pattern (applies to all content types with blank patterns below)');
  }

  /**
   * {@inheritdoc}
   */
  public function getPatterns() {
    $patterns = [];
    $languages = $this->languageManager->getLanguages();
    foreach ($this->getNodeTypeNames() as $node_type => $node_name) {
      if (count($languages) && $this->isContentTranslationEnabled($node_type)) {
        $patterns[$node_type] = $this->t('Default path pattern for @node_type (applies to all @node_type content types with blank patterns below)', array('@node_type' => $node_name));
        foreach ($languages as $language) {
          $patterns[$node_type . '_' . $language->getId()] = $this->t('Pattern for all @language @node_type paths', array('@node_type' => $node_name, '@language' => $language->getName()));
        }
      }
      else {
        $patterns[$node_type] = $this->t('Pattern for all @node_type paths', array('@node_type' => $node_name));
      }
    }
    return $patterns;
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('patternitems' => array('content/[node:title]')) + parent::defaultConfiguration();
  }

  /**
   * Wraps node_type_get_names().
   *
   * @return array
   *   An array of node type names, keyed by type.
   */
  protected function getNodeTypeNames() {
    return array_map(function ($bundle_info) {
      return $bundle_info['label'];
    }, $this->entityManager->getBundleInfo('node'));
  }

  /**
   * Wraps content_translation_enabled().
   *
   * @param string $node_type
   *   the node type.
   *
   * @return bool
   *   tRUE if content translation is enabled for the content type.
   */
  protected function isContentTranslationEnabled($node_type) {
    return $this->moduleHandler->moduleExists('content_translation') && content_translation_enabled('node', $node_type);
  }

}
