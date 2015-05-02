<?php

/**
 * @file
 * Contains \Drupal\pathauto\Plugin\AliasType\UserAliasType.
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
 * A pathauto alias type plugin for user entities.
 *
 * @AliasType(
 *   id = "user",
 *   label = @Translation("User"),
 *   types = {"user"},
 *   provider = "user",
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
    $this->t('Pattern for user account page paths');
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('default' => array('users/[user:name]')) + parent::defaultConfiguration();
  }

  /**
   * {@inheritdoc}
   */
  public function batchUpdate(&$context) {
    if (!isset($context['sandbox']['current'])) {
      $context['sandbox']['count'] = 0;
      $context['sandbox']['current'] = 0;
    }

    $query = db_select('users', 'u');
    $query->leftJoin('url_alias', 'ua', "CONCAT('user/', u.uid) = ua.source");
    $query->addField('u', 'uid');
    $query->isNull('ua.source');
    $query->condition('u.uid', $context['sandbox']['current'], '>');
    $query->orderBy('u.uid');
    $query->addTag('pathauto_bulk_update');
    $query->addMetaData('entity', 'user');

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
    $uids = $query->execute()->fetchCol();

    pathauto_user_update_alias_multiple($uids, 'bulkupdate', array());
    $context['sandbox']['count'] += count($uids);
    $context['sandbox']['current'] = max($uids);
    $context['message'] = t('Updated alias for user @uid.', array('@uid' => end($uids)));

    if ($context['sandbox']['count'] != $context['sandbox']['total']) {
      $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
    }
  }

}
