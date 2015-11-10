<?php

/**
 * @file
 * Contains Drupal\pathauto\AliasTypeInterface
 */

namespace Drupal\pathauto;

use Drupal\Component\Plugin\ConfigurablePluginInterface;
use Drupal\Core\Plugin\ContextAwarePluginInterface;
use Drupal\Core\Plugin\PluginFormInterface;

/**
 * Provides an interface for pathauto alias types.
 */
interface AliasTypeInterface extends ContextAwarePluginInterface, ConfigurablePluginInterface, PluginFormInterface {

  /**
   * Get the label.
   *
   * @return string
   *   The label.
   */
  public function getLabel();

  /**
   * Get the token types.
   *
   * @return string[]
   *   The token types.
   */
  public function getTokenTypes();

  /**
   * Returns the source prefix; used for bulk delete.
   *
   * @return string
   *   The source path prefix.
   */
  public function getSourcePrefix();

  /**
   * Determines if this plugin type can apply a given object.
   *
   * @param $object
   *   The object used to determine if this plugin can apply.
   *
   * @return bool
   */
  public function applies($object);

}
