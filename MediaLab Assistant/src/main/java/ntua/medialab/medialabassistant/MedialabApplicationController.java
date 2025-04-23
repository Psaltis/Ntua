package ntua.medialab.medialabassistant;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.io.IOException;
import java.time.LocalDate;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public class MedialabApplicationController {

    @FXML
    private Label CompletedTasks;

    @FXML
    private Label DelayedTasks;

    @FXML
    private Label DueWeekTasks;

    @FXML
    private Label TotalTaks;

    @FXML
    private Button addCategoriesButton;

    @FXML
    private Button addPriorBut;

    @FXML
    private Button addReminderButton;

    @FXML
    private Button addTasksButton;

    @FXML
    private Label completedTasksLabel;

    @FXML
    private Label delayedTasksLabel;

    @FXML
    private Button editCatBut;

    @FXML
    private Button editPriorBut;

    @FXML
    private ScrollPane reminderScrollPane;

    @FXML
    private VBox reminderVBox;

    @FXML
    private TextField searchbar;

    @FXML
    private ScrollPane searchResultsScrollPane;

    @FXML
    private VBox searchResultsVBox;

    @FXML
    private ScrollPane taskScrollPane;

    @FXML
    private VBox taskVBox;

    @FXML
    private Label totalTasksLabel;

    @FXML
    public void initialize() {
        loadData();
        updateTaskList();
        updateReminderList();
        updateTaskStatistics();
        checkTriggeredReminders();
        reminderScrollPane.setFitToWidth(true);
        taskScrollPane.setFitToWidth(true);
        searchbar.textProperty().addListener((observable, oldValue, newValue) -> {handleSearch(null);});
    }
    private void loadData() {
        CategoryManager.ensureInitialized(); // This will load categories from the file
        PriorityManager.ensureInitialized(); // This will load priorities from the file
        TaskManager.ensureInitialized(); // This will load tasks from the file
        ReminderManager.ensureInitialized(); // This will load reminders from the file
        TaskManager.updateOverdueTasks(); // Update delayed tasks
    }

    @FXML
    void handleAddCategories(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("AddCategory.fxml"));
            Parent root = loader.load();
            Stage stage = new Stage();
            stage.setTitle("Add New Category");
            stage.setScene(new Scene(root));
            stage.initOwner(((Node) event.getSource()).getScene().getWindow());
            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Failed to load the Add Category window.");
        }
    }

    @FXML
    void handleAddPriority(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("AddPriority.fxml"));
            Parent root = loader.load();
            Stage stage = new Stage();
            stage.setTitle("Add New Priority");
            stage.setScene(new Scene(root));
            stage.initOwner(((Node) event.getSource()).getScene().getWindow());
            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Failed to load the Add Priority window.");
        }
    }

    @FXML
    void handleAddReminder(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("AddReminder.fxml"));
            Parent root = loader.load();
            Stage stage = new Stage();
            stage.setTitle("Add Reminder");
            stage.setScene(new Scene(root));
            stage.showAndWait(); // Wait for the window to close
            updateReminderList(); // Refresh the reminder list
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    void handleAddTask(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("AddTask.fxml"));
            Parent root = loader.load();
            Stage stage = new Stage();
            stage.setTitle("Add Task");
            stage.setScene(new Scene(root));
            stage.showAndWait(); // Wait for the window to close
            updateTaskList(); // Refresh the task list
        } catch (IOException e) {
            e.printStackTrace();
        }
        updateTaskList();
        updateTaskStatistics();
        updateReminderList();
    }

    @FXML
    void handleEditCat(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("EditCategory.fxml"));
            Parent root = loader.load();
            Stage stage = new Stage();
            stage.setTitle("Edit Category");
            stage.setScene(new Scene(root));
            stage.initOwner(((Node) event.getSource()).getScene().getWindow());
            stage.showAndWait();
            updateTaskList();
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Failed to load the Edit Category window.");
        }
        updateTaskList();
        updateTaskStatistics();
        updateReminderList();
    }

    @FXML
    void handleEditPrior(ActionEvent event) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("EditPriority.fxml"));
            Parent root = loader.load();
            Stage stage = new Stage();
            stage.setTitle("Edit Priority");
            stage.setScene(new Scene(root));
            stage.initOwner(((Node) event.getSource()).getScene().getWindow());
            stage.showAndWait();
            updateTaskList();
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Failed to load the Edit Priority window.");
        }
        updateTaskList();
        updateTaskStatistics();
        updateReminderList();
    }

    @FXML
    void handleSearch(ActionEvent event) {
        String searchText = searchbar.getText().trim().toLowerCase();
        String title = null, category = null, priority = null;

        System.out.println("Search Input: " + searchText);

        for (String word : searchText.split("\\s+")) { // Split input into words

            category = CategoryManager.getCategories().stream()
                    .filter(c -> c.toLowerCase().contains(word))
                    .findFirst().orElse(category);

            // Find the first priority that contains the word (case insensitive)
            priority = PriorityManager.getPriorities().stream()
                    .filter(p -> p.toLowerCase().contains(word))
                    .findFirst().orElse(priority);

            // If the word is not matched to a category or priority, treat it as title
            if (category == null && priority == null) {
                title = (title == null) ? word : title + " " + word;
            }
        }

        System.out.println("Searching with - Title: " + title + ", Category: " + category + ", Priority: " + priority);
        List<Task> results = SearchEngine.search(title, category, priority);

        if (searchText.isEmpty()) {
            updateTaskList();
        } else {
            updateSearchResults(results);
        }
    }


    public void updateSearchResults(List<Task> results) {
        taskVBox.getChildren().clear(); // Clear old results
        System.out.println("Updating UI with " + results.size() + " results."); // Debugging log

        Map<String, List<Task>> groupedTasks = results.stream()
                .collect(Collectors.groupingBy(Task::getCategory));

        for (Map.Entry<String, List<Task>> entry : groupedTasks.entrySet()) {
            String category = entry.getKey();
            List<Task> tasksInCategory = entry.getValue();

            System.out.println("Displaying Category: " + category + " with " + tasksInCategory.size() + " tasks.");

            Label categoryLabel = new Label(category);
            categoryLabel.setStyle("-fx-font-weight: bold; -fx-padding: 5px 0;");

            VBox categoryVBox = new VBox(10);
            categoryVBox.setPadding(new Insets(5, 0, 5, 0));

            for (Task task : tasksInCategory) {
                try {
                    FXMLLoader loader = new FXMLLoader(getClass().getResource("TaskItem.fxml"));
                    HBox taskItem = loader.load();
                    TaskItemController controller = loader.getController();
                    controller.setTask(task);
                    controller.setMainController(this);
                    categoryVBox.getChildren().add(taskItem);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            taskVBox.getChildren().addAll(categoryLabel, categoryVBox);
        }
    }

    public void updateTaskList() {
        taskVBox.getChildren().clear(); // Clear the existing task items
        // Group tasks by category using a Map
        Map<String, List<Task>> groupedTasks = TaskManager.getTasks().stream()
                .collect(Collectors.groupingBy(Task::getCategory));
        // Iterate through each category and create a section for it
        for (Map.Entry<String, List<Task>> entry : groupedTasks.entrySet()) {
            String category = entry.getKey();
            List<Task> tasksInCategory = entry.getValue();
            // Create a label for the category
            Label categoryLabel = new Label(category);
            categoryLabel.setStyle("-fx-font-weight: bold; -fx-padding: 5px 0;");
            // Create a VBox to hold tasks in this category
            VBox categoryVBox = new VBox(10); // Spacing between tasks
            categoryVBox.setPadding(new Insets(5, 0, 5, 0));
            // Add tasks to the category VBox
            for (Task task : tasksInCategory) {
                try {
                    FXMLLoader loader = new FXMLLoader(getClass().getResource("TaskItem.fxml"));
                    HBox taskItem = loader.load();
                    TaskItemController controller = loader.getController();
                    controller.setTask(task);
                    controller.setMainController(this); // Pass the main controller reference
                    categoryVBox.getChildren().add(taskItem);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            // Add the category label and its tasks to the main taskVBox
            taskVBox.getChildren().addAll(categoryLabel, categoryVBox);
        }
    }

    public void updateReminderList() {
        reminderVBox.getChildren().clear(); // Clear the existing reminder items

        for (Reminder reminder : ReminderManager.getReminders()) {
            try {
                FXMLLoader loader = new FXMLLoader(getClass().getResource("ReminderItem.fxml"));
                HBox reminderItem = loader.load(); // Load as HBox
                ReminderItemController controller = loader.getController();
                controller.setReminder(reminder); // Set the reminder for the controller
                controller.setMainController(this);
                reminderVBox.getChildren().add(reminderItem); // Add the item to the VBox
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public void updateTaskStatistics() {
        List<Task> tasks = TaskManager.getTasks();
        long totalTasks = tasks.size();
        long completedTasks = tasks.stream().filter(task -> task.getStatus() == Task.Status.Completed).count();
        long delayedTasks = tasks.stream().filter(task -> task.getStatus() == Task.Status.Delayed).count();
        long dueWeekTasks = tasks.stream()
                .filter(task -> task.getDeadline() != null) // Ensure the task has a deadline
                .filter(task -> !task.getStatus().equals(Task.Status.Completed)) // Exclude completed tasks
                .filter(task -> !task.getStatus().equals(Task.Status.Delayed)) // Exclude delayed tasks
                .filter(task -> task.getDeadline().isAfter(LocalDate.now()) && task.getDeadline().isBefore(LocalDate.now().plusWeeks(1)))
                .count();
        TotalTaks.setText(String.valueOf(totalTasks));
        CompletedTasks.setText(String.valueOf(completedTasks));
        DelayedTasks.setText(String.valueOf(delayedTasks));
        DueWeekTasks.setText(String.valueOf(dueWeekTasks));
    }

    private void checkTriggeredReminders(){
        for (Reminder reminder : ReminderManager.getReminders()){
            if(!reminder.getTrigger() && reminder.getReminderDate().isEqual(LocalDate.now())){
                showReimnderPopup(reminder);
                reminder.fireTrigger();
            }
        }
    }
    private void showReimnderPopup(Reminder reminder){
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("Reminder");
            alert.setHeaderText("Task Reminder");
            alert.setContentText("Reminder for task " + reminder.getTask().getTitle() + "\nDeadline: " + reminder.getTask().getDeadline());
            alert.show();
        });
    }
}