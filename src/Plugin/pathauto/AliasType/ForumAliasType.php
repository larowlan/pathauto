<?php

/**
 * @file
 * Contains \Drupal\pathauto\Plugin\AliasType\ForumAliasType.
 */

namespace Drupal\pathauto\Plugin\pathauto\AliasType;

use Drupal\Core\Entity\EntityManagerInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Language\LanguageInterface;
use Drupal\Core\Language\LanguageManagerInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\pathauto\AliasTypeBatchUpdateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * A pathauto alias type plugin for forum terms.
 *
 * @AliasType(
 *   id = "forum",
 *   label = @Translation("Forum"),
 *   types = {"term"},
 *   provider = "forum",
 * )
 */
class UserAliasType extends AliasTypeBase implements AliasTypeBatchUpdateInterface, ContainerFactoryPluginInterface {

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
    $this->t('Pattern for forums and forum containers');
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('patternitems' => array('[term:vocabulary]/[term:name]')) + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  public function batchUpdate(&$context) {
    if (!isset($context['sandbox']['current'])) {
      $context['sandbox']['count'] = 0;
      $context['sandbox']['current'] = 0;
    }

    $query = db_select('taxonomy_term_data', 'td');
    $query->leftJoin('url_alias', 'ua', "CONCAT('forum/', td.tid) = ua.source");
    $query->addField('td', 'tid');
    $query->isNull('ua.source');
    $query->condition('td.tid', $context['sandbox']['current'], '>');
    $query->condition('td.vid', 'forums');
    $query->orderBy('td.tid');
    $query->addTag('pathauto_bulk_update');
    $query->addMetaData('entity', 'taxonomy_term');

    // Get the total amount of items to process.
    if (!isset($context['sandbox']['total'])) {
      $context['sandbox']['total'] = $query->countQuery()
        ->execute()
        ->fetchField();

      // If there are no nodes to update, the stop immediately.
      if (!$context['sandbox']['total']) {
        $context['finished'] = 1;
        return;
      }
    }

    $query->range(0, 25);
    $tids = $query->execute()->fetchCol();

    pathauto_taxonomy_term_update_alias_multiple($tids, 'bulkupdate');
    $context['sandbox']['count'] += count($tids);
    $context['sandbox']['current'] = max($tids);
    $context['message'] = t('Updated alias for forum @tid.', array('@tid' => end($tids)));

    if ($context['sandbox']['count'] != $context['sandbox']['total']) {
      $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
    }
  }

}
