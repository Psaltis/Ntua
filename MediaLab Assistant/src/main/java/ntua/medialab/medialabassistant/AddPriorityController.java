package ntua.medialab.medialabassistant;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

public class AddPriorityController {

    @FXML
    private Button addPriorityButton;

    @FXML
    private Button closeButton;

    @FXML
    private TextField priorityTextField;

    @FXML
    void handleAddPriority() {
        String newPriority = priorityTextField.getText().trim();
        if (!newPriority.isEmpty()) {
            try {
                PriorityManager.addPriority(newPriority);
                System.out.println("Priority added: " + newPriority);
                handleClose();
            } catch (IllegalArgumentException e) {
                System.err.println("Error: " + e.getMessage());
            }
        } else {
            System.err.println("Error: Priority name cannot be empty.");
        }
    }
    @FXML
    void handleClose() {
        Stage stage = (Stage) priorityTextField.getScene().getWindow();
        stage.close();
    }
}
