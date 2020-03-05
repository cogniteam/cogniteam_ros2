#include <cognitao_ros2/server_example/MinimalActionServerExample.h>

MinimalActionServerExample::MinimalActionServerExample(rclcpp::Node::SharedPtr node)
    : MinimalActionServer(node) {}

void MinimalActionServerExample::execute(const std::shared_ptr<GoalHandleActionType> goal_handle)
{
    rclcpp::Rate loop_rate(1);
    const auto goal = goal_handle->get_goal();
    auto feedback = std::make_shared<actionType::Feedback>();
    auto result = std::make_shared<actionType::Result>();

    string action_type = goal->goal.actiontype;

    cout << " got action : " << action_type << endl;
    std::map<std::string, std::string> parameters;
    for (auto const &param : goal->goal.parameters)
    {
        parameters[param.key] = param.val;
        cout << " val is " << param.val << endl;
    }

    if (action_type == "wait")
    {

        int totalLoop = atoi(parameters["time"].c_str());
        cout << " inside wait " << endl;
        for (int i = 0; i < totalLoop; i++)
        {
            cout << " executing " << action_type << " i " << i << endl;
            goal_handle->publish_feedback(feedback);
            loop_rate.sleep();
            if (goal_handle->is_canceling())
            {
                cout << "Goal Canceled " << endl;
                result->resultvalue = false;
                goal_handle->set_succeeded(result);
                return;
            }
        }
    }

    // Check if goal is done
    if (rclcpp::ok())
    {
        cout << " set Goal Succeeded " << endl;
        result->resultvalue = true;
        goal_handle->set_succeeded(result);
    }
}

void MinimalActionServerExample::handle_accepted(const std::shared_ptr<GoalHandleActionType> goal_handle)
{
    using namespace std::placeholders;
    // this needs to return quickly to avoid blocking the executor, so spin up a new thread
    std::thread{std::bind(&MinimalActionServerExample::execute, this, _1), goal_handle}.detach();
}