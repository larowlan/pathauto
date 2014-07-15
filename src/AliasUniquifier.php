<?php

/**
 * @file
 * Contains Drupal\pathauto\AliasUniquifier
 */

namespace Drupal\pathauto;

use Drupal\Component\Utility\Unicode;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Language\LanguageInterface;

/**
 * Provides a utility for creating a unique path alias.
 */
class AliasUniquifier implements AliasUniquifierInterface {

  /**
   * Config factory.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * The alias storage helper.
   *
   * @var \Drupal\pathauto\AliasStorageHelperInterface
   */
  protected $aliasStorageHelper;

  /**
   * The module handler.
   *
   * @var \Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $moduleHandler;

  /**
   * Creates a new AliasUniquifier.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The config factory.
   * @param \Drupal\pathauto\AliasStorageHelperInterface $alias_storage_helper
   *   The alias storage helper.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   */
  public function __construct(ConfigFactoryInterface $config_factory, AliasStorageHelperInterface $alias_storage_helper, ModuleHandlerInterface $module_handler) {
    $this->configFactory = $config_factory;
    $this->aliasStorageHelper = $alias_storage_helper;
    $this->moduleHandler = $module_handler;
  }

  /**
   * {@inheritdoc}
   */
  public function uniquify(&$alias, $source, $langcode) {
    $config = $this->configFactory->get('pathauto.settings');

    if (!$this->isReserved($alias, $source, $langcode)) {
      return;
    }

    // If the alias already exists, generate a new, hopefully unique, variant.
    $maxlength = min($config->get('max_length'), $this->aliasStorageHelper->getAliasSchemaMaxlength());
    $separator = $config->get('separator');
    $original_alias = $alias;

    $i = 0;
    do {
      // Append an incrementing numeric suffix until we find a unique alias.
      $unique_suffix = $separator . $i;
      $alias = Unicode::truncate($original_alias, $maxlength - Unicode::strlen($unique_suffix, TRUE)) . $unique_suffix;
      $i++;
    } while ($this->isReserved($alias, $source, $langcode));
  }

  /**
   * {@inheritdoc}
   */
  public function isReserved($alias, $source, $langcode = LanguageInterface::LANGCODE_NOT_SPECIFIED) {
    $args = array(
      $alias,
      $source,
      $langcode,
    );
    $implementations = $this->moduleHandler->getImplementations('pathauto_is_alias_reserved');
    foreach ($implementations as $module) {

      $result = $this->moduleHandler->invoke($module, 'pathauto_is_alias_reserved', $args);

      if (!empty($result)) {
        // As soon as the first module says that an alias is in fact reserved,
        // then there is no point in checking the rest of the modules.
        return TRUE;
      }
    }

    return FALSE;
  }

}
