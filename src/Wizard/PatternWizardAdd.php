<?php
/**
 * @file
 * Contains \Drupal\pathauto\Wizard\PatternWizardAdd.
 */

namespace Drupal\pathauto\Wizard;

/**
 * Custom override for create form.
 */
class PatternWizardAdd extends PatternWizard {

  /**
   * {@inheritdoc}
   */
  public function getRouteName() {
    return 'entity.pathauto_pattern.edit_form';
  }

}
