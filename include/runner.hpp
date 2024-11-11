#pragma once

namespace Runner {
    //run each segment of the code once
    void executeVars();
    void executeStart();
    void executeUpdate();

    //run the whole program and loop executeUpdate
    void execute();
}