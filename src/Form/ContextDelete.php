<?php
/**
 * @file
 * Contains \Drupal\pathauto\Form\ContextDelete.
 */

namespace Drupal\pathauto\Form;


use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\ctools\Form\RelationshipDelete;

class ContextDelete extends RelationshipDelete {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'pathauto_relationship_delete';
  }

  /**
   * {@inheritdoc}
   */
  public function getCancelUrl($cached_values = []) {
    return new Url('entity.pathauto_pattern.edit_form', ['machine_name' => $this->machine_name, 'step' => 'contexts']);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $cached_values = $this->tempstore->get($this->tempstore_id)->get($this->machine_name);;
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    $pattern->removeContext($this->id);
    $this->tempstore->get($this->tempstore_id)->set($this->machine_name, $cached_values);
    parent::submitForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function getContexts($cached_values) {
    /** @var \Drupal\pathauto\PathautoPatternInterface $pattern */
    $pattern = $cached_values['pathauto_pattern'];
    return $pattern->getContexts();
  }

}
