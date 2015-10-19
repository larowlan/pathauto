<?php

/**
 * @file
 * Contains Drupal\pathauto\PathautoPatternInterface.
 */

namespace Drupal\pathauto;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining Pathauto pattern entities.
 */
interface PathautoPatternInterface extends ConfigEntityInterface {

  /**
   * Get the tokenized pattern used during alias generation.
   *
   * @return string
   */
  public function getPattern();

  /**
   * Set the tokenized pattern to use during alias generation.
   *
   * @param string $pattern
   */
  public function setPattern($pattern);

  /**
   * Gets the type of this pattern.
   *
   * @return string
   */
  public function getType();

  /**
   * Gets the weight of this pattern (compared to other patterns of this type).
   *
   * @return int
   */
  public function getWeight();

  /**
   * Sets the weight of this pattern (compared to other patterns of this type).
   *
   * @param int $weight
   *   The weight of the variant.
   *
   * @return $this
   */
  public function setWeight($weight);

  /**
   * Gets the selection condition collection.
   *
   * @return \Drupal\Core\Condition\ConditionInterface[]|\Drupal\Core\Condition\ConditionPluginCollection
   */
  public function getSelectionConditions();

  /**
   * Adds selection criteria.
   *
   * @param array $configuration
   *   Configuration of the selection criteria.
   *
   * @return string
   *   The condition id of the new criteria.
   */
  public function addSelectionCondition(array $configuration);

  /**
   * Gets selection criteria by condition id.
   *
   * @param string $condition_id
   *   The id of the condition.
   *
   * @return \Drupal\Core\Condition\ConditionInterface
   */
  public function getSelectionCondition($condition_id);

  /**
   * Removes selection criteria by condition id.
   *
   * @param string $condition_id
   *   The id of the condition.
   *
   * @return $this
   */
  public function removeSelectionCondition($condition_id);

  /**
   * Gets the selection logic used by the criteria (ie. "and" or "or").
   *
   * @return string
   *   Either "and" or "or"; represents how the selection criteria are combined.
   */
  public function getSelectionLogic();

}
