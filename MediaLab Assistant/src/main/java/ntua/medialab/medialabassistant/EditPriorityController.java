package ntua.medialab.medialabassistant;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

public class EditPriorityController {

    @FXML
    private Button closeButton;

    @FXML
    private TextField newPriorityNameField;

    @FXML
    private ChoiceBox<String> priorityChoiceBox;

    @FXML
    private Button removePriorityButton;

    @FXML
    private Button savePriorityButton;
    @FXML
    private void initialize() {
        priorityChoiceBox.getItems().addAll(PriorityManager.getPriorities());
    }
    @FXML
    void handleClose() {
        Stage stage = (Stage) priorityChoiceBox.getScene().getWindow();
        stage.close();
    }

    @FXML
    void handleRemovePriority(ActionEvent event) {
        String selectedPriority = priorityChoiceBox.getValue();

        if (selectedPriority != null) {
            try {
                PriorityManager.removePriority(selectedPriority);
                System.out.println("Category removed: " + selectedPriority);
                handleClose();
            } catch (IllegalArgumentException e) {
                System.err.println("Error: " + e.getMessage());
            }
        } else {
            System.err.println("Error: Please select a valid priority to remove.");
        }

    }

    @FXML
    void handleSavePriority(ActionEvent event) {

        String selectedPriority = priorityChoiceBox.getValue();
        String newPriorityName = newPriorityNameField.getText().trim();

        if (selectedPriority != null && !newPriorityName.isEmpty()) {
            try {
                PriorityManager.editPriority(selectedPriority, newPriorityName);
                System.out.println("Priority updated: " + selectedPriority + " -> " + newPriorityName);
                handleClose();
            } catch (IllegalArgumentException e) {
                System.err.println("Error: " + e.getMessage());
            }
        } else {
            System.err.println("Error: Please select a valid priority and enter a new name.");
        }
    }

}
