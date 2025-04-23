package ntua.medialab.medialabassistant;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import org.json.JSONArray;
import java.nio.file.Files;
import java.nio.file.Paths;

public class PriorityManager {
    private static final String FILE_NAME = "medialab/priorities.json";
    private static final String DEFAULT = "Default";
    private static List<String> priorities = null;
    // Ensure the priority list is initialized
    public static void ensureInitialized() {
        if (priorities == null || priorities.isEmpty()) {
            priorities = loadPriorities();
            if (priorities == null) {
                priorities = new ArrayList<>(); // Default empty list if file is missing
            }
            if (!priorities.contains(DEFAULT)) {
                priorities.add(DEFAULT); // Ensure "Default" is always present
            }
        }
    }
    // Add a new priority
    public static void addPriority(String priority) {
        if (priorities.contains(priority)) {
            throw new IllegalArgumentException("Priority '" + priority + "' already exists.");
        }
        priorities.add(priority);
    }
    // Remove a priority
    public static void removePriority(String priority) {
        if (priority.equals(DEFAULT)) {
            throw new IllegalArgumentException("Default priority level cannot be removed.");
        }
        if (!priorities.contains(priority)) {
            throw new IllegalArgumentException("Priority level '" + priority + "' does not exist.");
        }
        TaskManager.removeTasksByPriority(priority); // Notify TaskManager to remove tasks
        priorities.remove(priority);
    }
    // Edit a priority (rename)
    public static void editPriority(String oldPriority, String newPriority) {
        if (oldPriority.equals(DEFAULT)) {
            throw new IllegalArgumentException("Default priority level cannot be edited.");
        }
        if (!priorities.contains(oldPriority)) {
            throw new IllegalArgumentException("Priority level '" + oldPriority + "' does not exist.");
        }
        if (priorities.contains(newPriority)) {
            throw new IllegalArgumentException("Priority level '" + newPriority + "' already exists.");
        }
        else{
            int index = priorities.indexOf(oldPriority);
            priorities.set(index, newPriority); // Replace old priority with new one
            TaskManager.updateTasksForRenamedPriority(oldPriority, newPriority); // Notify TaskManager
        }
    }
    public static void setPriorities(){
        savePriorities();
    }
    public static List<String> getPriorities() {
        if (priorities == null || priorities.isEmpty()) {
            System.out.println("PriorityManager: No priorities loaded!");
        } else {
            System.out.println("PriorityManager: Loaded priorities - " + priorities);
        }
        return priorities;
    }
    private static void savePriorities() {
        JSONArray jsonArray = new JSONArray(priorities);
        try (FileWriter file = new FileWriter(FILE_NAME)) {
            file.write(jsonArray.toString(2));
            file.flush();
        } catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("Failed to save priorities to file: " + FILE_NAME, e);
        }
    }
    private static List<String> loadPriorities() {
        if (!Files.exists(Paths.get(FILE_NAME))) {
            return null;
        }
        try {
            String content = new String(Files.readAllBytes(Paths.get(FILE_NAME)));
            JSONArray jsonArray = new JSONArray(content);
            List<String> loadedPriorities = new ArrayList<>();
            for (int i = 0; i < jsonArray.length(); i++) {
                loadedPriorities.add(jsonArray.getString(i));
            }
            return loadedPriorities;
        } catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("Failed to load priorities from file: " + FILE_NAME, e);
        }
    }
}
