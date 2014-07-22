<?php

/**
 * @file
 * Contains Drupal\pathauto\MessengerInterface
 */

namespace Drupal\pathauto;

/**
 * Provides and
 */
interface MessengerInterface {

  /**
   * Adds a message.
   *
   * @param string $message
   *   The message to add.
   * @param string $op
   *   (optional) The operation being performed.
   */
  public function addMessage($message, $op = NULL);

}
