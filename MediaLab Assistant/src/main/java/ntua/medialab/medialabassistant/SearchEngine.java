package ntua.medialab.medialabassistant;

import java.util.List;
import java.util.ArrayList;
import java.util.stream.Collectors;

public class SearchEngine {
    public static List<Task> search(String title, String category, String priority) {
        List<Task> tasks = TaskManager.getTasks();
        System.out.println("Total Tasks in System: " + tasks.size());

        return tasks.stream()
                .filter(task -> {
                    boolean matchesTitle = (title == null || title.isEmpty() || task.getTitle().toLowerCase().contains(title.toLowerCase()));
                    boolean matchesCategory = (category == null || category.isEmpty() || task.getCategory().equalsIgnoreCase(category));
                    boolean matchesPriority = (priority == null || priority.isEmpty() || task.getPriority().equalsIgnoreCase(priority));
                    boolean atLeastOneFilterUsed = (title != null && !title.isEmpty()) ||
                            (category != null && !category.isEmpty()) ||
                            (priority != null && !priority.isEmpty());
                    boolean matches = (matchesTitle && title != null) ||
                            (matchesCategory && category != null) ||
                            (matchesPriority && priority != null);
                    System.out.println("Checking Task: " + task.getTitle() + " | Category: " + task.getCategory() +
                            " | Priority: " + task.getPriority() + " --> Matches: " + matches);
                    return atLeastOneFilterUsed && matches;
                })
                .collect(Collectors.toList());
    }
}
//https://stackoverflow.com/questions/122105/how-to-filter-a-java-collection-based-on-predicate/1385698#1385698