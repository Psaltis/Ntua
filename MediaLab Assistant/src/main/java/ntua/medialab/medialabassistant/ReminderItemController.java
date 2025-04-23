package ntua.medialab.medialabassistant;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;

public class ReminderItemController {

    @FXML
    private Label titleLabel;

    @FXML
    private Label deadlineLabel;

    @FXML
    private Button removeReminder;
    private MedialabApplicationController mainController;
    private Reminder reminder;
    public void setMainController(MedialabApplicationController mainController) {
        this.mainController = mainController;
    }
    public void setReminder(Reminder reminder) {
        this.reminder = reminder;
        titleLabel.setText(reminder.getTask().getTitle());
        deadlineLabel.setText(reminder.getReminderDate().toString());
    }

    @FXML
    private void handleRemoveReminder() {
        if (reminder != null) {
        ReminderManager.removeReminder(reminder); // Pass the specific reminder to be removed
        System.out.println("Reminder removed for task: " + reminder.getTask().getTitle() +
                " on date: " + reminder.getReminderDate().toString());
    } else {
        System.err.println("Error: Reminder not found.");
    }
        mainController.updateReminderList();
    }
}