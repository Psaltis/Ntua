package ntua.medialab.medialabassistant;

import org.json.JSONArray;
import org.json.JSONObject;
import java.io.*;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.nio.file.Files;
import java.nio.file.Paths;

/**
 * Manages a list of tasks, providing functionality to add, remove, edit, and update tasks.
 * Tasks are loaded from and saved to a JSON file.
 */
public class TaskManager {
    private static final String FILE_NAME = "medialab/tasks.json";
    private static final DateTimeFormatter DATE_FORMAT = DateTimeFormatter.ISO_LOCAL_DATE;
    private static List<Task> tasks = null;

    /**
     * Ensures that the task list is initialized. If the list is null, it loads tasks from the file.
     * If the file does not exist, it initializes an empty list.
     */
    public static void ensureInitialized() {
        if (tasks == null) {
            tasks = loadTasks();
            if (tasks == null) {
                tasks = new ArrayList<>();
            }
        }
    }

    /**
     * Adds a new task to the task list.
     *
     * @param task The task to be added.
     */
    public static void addTask(Task task) {
        tasks.add(task);
    }

    /**
     * Saves the current list of tasks to the file.
     */
    public static void setTasks() {
        saveTasks();
    }

    /**
     * Returns the current list of tasks.
     *
     * @return The list of tasks.
     */
    public static List<Task> getTasks() {
        return tasks;
    }

    /**
     * Removes a specific task from the task list and removes any associated reminders.
     *
     * @param task The task to be removed.
     */
    public static void removeTask(Task task) {
        tasks.remove(task);
        ReminderManager.removeRemindersForTask(task);
    }

    /**
     * Removes all tasks that belong to a specific category and removes any associated reminders.
     *
     * @param category The category of tasks to be removed.
     */
    public static void removeTasksByCategory(String category) {
        Iterator<Task> iterator = tasks.iterator();
        while (iterator.hasNext()) {
            Task task = iterator.next();
            if (task.getCategory().equals(category)) {
                ReminderManager.removeRemindersForTask(task);
                iterator.remove();
            }
        }
    }

    /**
     * Removes all tasks that have a specific priority and removes any associated reminders.
     *
     * @param priority The priority of tasks to be removed.
     */
    public static void removeTasksByPriority(String priority) {
        Iterator<Task> iterator = tasks.iterator();
        while (iterator.hasNext()) {
            Task task = iterator.next();
            if (task.getPriority().equals(priority)) {
                ReminderManager.removeRemindersForTask(task);
                iterator.remove();
            }
        }
    }

    /**
     * Edits the details of a specific task.
     *
     * @param task The task to be edited.
     * @param newTitle The new title for the task (can be null to leave unchanged).
     * @param newDescription The new description for the task (can be null to leave unchanged).
     * @param newCategory The new category for the task (can be null to leave unchanged).
     * @param newPriority The new priority for the task (can be null to leave unchanged).
     * @param newDeadline The new deadline for the task (can be null to leave unchanged).
     * @param newStatus The new status for the task (can be null to leave unchanged).
     * @throws IllegalArgumentException if the task is not found in the list.
     */
    public static void editTask(Task task, String newTitle, String newDescription, String newCategory, String newPriority, LocalDate newDeadline, Task.Status newStatus) {
        if (!tasks.contains(task)) {
            throw new IllegalArgumentException("Task not found.");
        }
        if (newTitle != null) task.setTitle(newTitle);
        if (newDescription != null) task.setDescription(newDescription);
        if (newCategory != null) task.setCategory(newCategory);
        if (newPriority != null) task.setPriority(newPriority);
        if (newDeadline != null) task.setDeadline(newDeadline);
        if (newStatus != null) task.setStatus(newStatus);
    }

    /**
     * Updates the category of all tasks that have a specific old category to a new category.
     *
     * @param oldCategory The old category to be replaced.
     * @param newCategory The new category to replace the old one.
     */
    public static void updateTasksForRenamedCategory(String oldCategory, String newCategory) {
        for (Task task : tasks) {
            if (task.getCategory().equals(oldCategory)) {
                task.setCategory(newCategory);
            }
        }
    }

    /**
     * Updates the priority of all tasks that have a specific old priority to a new priority.
     *
     * @param oldPriority The old priority to be replaced.
     * @param newPriority The new priority to replace the old one.
     */
    public static void updateTasksForRenamedPriority(String oldPriority, String newPriority) {
        for (Task task : tasks) {
            if (task.getPriority().equals(oldPriority)) {
                task.setPriority(newPriority);
            }
        }
    }

    /**
     * Updates the status of tasks that are overdue and not completed to "Delayed".
     */
    public static void updateOverdueTasks() {
        LocalDate today = LocalDate.now();
        for (Task task : tasks) {
            if (task.getDeadline() != null
                    && task.getDeadline().isBefore(today)
                    && task.getStatus() != Task.Status.Completed) {
                task.setStatus(Task.Status.Delayed);
            }
        }
    }
    
    private static void saveTasks() {
        JSONArray jsonArray = new JSONArray();
        for (Task task : tasks) {
            JSONObject jsonTask = new JSONObject();
            jsonTask.put("title", task.getTitle());
            jsonTask.put("description", task.getDescription());
            jsonTask.put("category", task.getCategory());
            jsonTask.put("priority", task.getPriority());
            jsonTask.put("deadline", task.getDeadline() != null ? task.getDeadline().format(DATE_FORMAT) : JSONObject.NULL);
            jsonTask.put("status", task.getStatus().toString());
            jsonArray.put(jsonTask);
        }
        try (FileWriter file = new FileWriter(FILE_NAME)) {
            file.write(jsonArray.toString(2));
            file.flush();
        } catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("Failed to save tasks to file: " + FILE_NAME, e);
        }
    }
    private static List<Task> loadTasks() {
        if (!Files.exists(Paths.get(FILE_NAME))) {
            return null;
        }
        try {
            String content = new String(Files.readAllBytes(Paths.get(FILE_NAME)));
            JSONArray jsonArray = new JSONArray(content);
            List<Task> loadedTasks = new ArrayList<>();
            for (int i = 0; i < jsonArray.length(); i++) {
                JSONObject jsonTask = jsonArray.getJSONObject(i);
                Task task = new Task(
                        jsonTask.getString("title"),
                        jsonTask.getString("description"),
                        jsonTask.getString("category"),
                        jsonTask.getString("priority"),
                        jsonTask.isNull("deadline") ? null : LocalDate.parse(jsonTask.getString("deadline"), DATE_FORMAT),
                        Task.Status.valueOf(jsonTask.getString("status"))
                );
                loadedTasks.add(task);
            }
            return loadedTasks;
        } catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("Failed to load tasks from file: " + FILE_NAME, e);
        }
    }
}