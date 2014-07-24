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
use Symfony\Component\Routing\Exception\ResourceNotFoundException;
use Symfony\Component\Routing\Matcher\UrlMatcherInterface;

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
   * The url matcher service.
   *
   * @var \Symfony\Component\Routing\Matcher\UrlMatcherInterface
   */
  protected $urlMatcher;

  /**
   * Creates a new AliasUniquifier.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The config factory.
   * @param \Drupal\pathauto\AliasStorageHelperInterface $alias_storage_helper
   *   The alias storage helper.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler.
   * @param \Symfony\Component\Routing\Matcher\UrlMatcherInterface $url_matcher
   *   The url matcher service.
   */
  public function __construct(ConfigFactoryInterface $config_factory, AliasStorageHelperInterface $alias_storage_helper, ModuleHandlerInterface $module_handler, UrlMatcherInterface $url_matcher) {
    $this->configFactory = $config_factory;
    $this->aliasStorageHelper = $alias_storage_helper;
    $this->moduleHandler = $module_handler;
    $this->urlMatcher = $url_matcher;
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
    // First check whether the alias exists for another source.
    if ($this->aliasStorageHelper->exists($alias, $source, $langcode)) {
      return TRUE;
    }
    // Then check if there is a route with the same path.
    if ($this->isRoute($alias)) {
      return TRUE;
    }
    // Finally check if any other modules have reserved the alias.
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

  /**
   * Verify if the given path is a valid route.
   *
   * Taken from menu_execute_active_handler().
   *
   * @param string $path
   *   A string containing a relative path.
   *
   * @return bool
   *   TRUE if the path already exists.
   */
  public function isRoute($path) {
    if (is_file(DRUPAL_ROOT . '/' . $path) || is_dir(DRUPAL_ROOT . '/' . $path)) {
      // Do not allow existing files or directories to get assigned an automatic
      // alias. Note that we do not need to use is_link() to check for symbolic
      // links since this returns TRUE for either is_file() or is_dir() already.
      return TRUE;
    }

    try {
      $this->urlMatcher->match('/' . $path);
      return TRUE;
    }
    catch (ResourceNotFoundException $e) {
      return FALSE;
    }
  }

}
