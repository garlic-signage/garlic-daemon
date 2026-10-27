add_test([=[CmdParser.NoArguments]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.NoArguments]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.NoArguments]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:36]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.OptionWithValue]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.OptionWithValue]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.OptionWithValue]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:45]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.SeveralOptions]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.SeveralOptions]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.SeveralOptions]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:54]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.OptionsWithoutValue]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.OptionsWithoutValue]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.OptionsWithoutValue]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:67]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.LastValueWins]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.LastValueWins]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.LastValueWins]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:77]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.MissingValueIsError]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.MissingValueIsError]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.MissingValueIsError]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:84]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.UnknownOptionIsError]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.UnknownOptionIsError]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.UnknownOptionIsError]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:91]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.ArgumentWithoutOptionIsError]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.ArgumentWithoutOptionIsError]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.ArgumentWithoutOptionIsError]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:98]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.ProgramNameIsNotParsed]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.ProgramNameIsNotParsed]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.ProgramNameIsNotParsed]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:105]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[CmdParser.HelpTextListsAllOptions]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=CmdParser.HelpTextListsAllOptions]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[CmdParser.HelpTextListsAllOptions]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_cmd_parser.cpp:116]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.DefaultValues]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.DefaultValues]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.DefaultValues]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:46]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.ReadsAllValues]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.ReadsAllValues]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.ReadsAllValues]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:58]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.MissingPlayerGroupKeepsPlayerDefaults]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.MissingPlayerGroupKeepsPlayerDefaults]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.MissingPlayerGroupKeepsPlayerDefaults]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:89]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.MissingSingleValueKeepsDefault]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.MissingSingleValueKeepsDefault]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.MissingSingleValueKeepsDefault]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:97]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.ShippedConfigFileIsValid]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.ShippedConfigFileIsValid]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.ShippedConfigFileIsValid]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:108]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.EnvironmentEntryWithoutNameOrValueThrows]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.EnvironmentEntryWithoutNameOrValueThrows]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.EnvironmentEntryWithoutNameOrValueThrows]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:141]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.EnvironmentEntryWithEmptyValueIsAllowed]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.EnvironmentEntryWithEmptyValueIsAllowed]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.EnvironmentEntryWithEmptyValueIsAllowed]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:152]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.CommandLineOverridesConfigFile]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.CommandLineOverridesConfigFile]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.CommandLineOverridesConfigFile]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:159]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.CommandLineWithoutOptionsKeepsValues]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.CommandLineWithoutOptionsKeepsValues]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.CommandLineWithoutOptionsKeepsValues]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:179]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.HeartbeatTimeoutZeroIsAllowed]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.HeartbeatTimeoutZeroIsAllowed]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.HeartbeatTimeoutZeroIsAllowed]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:189]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Configuration.InvalidHeartbeatTimeoutThrows]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Configuration.InvalidHeartbeatTimeoutThrows]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Configuration.InvalidHeartbeatTimeoutThrows]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_configuration.cpp:198]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[DaemonTest.ReceivedMessageIsDispatched]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=DaemonTest.ReceivedMessageIsDispatched]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DaemonTest.ReceivedMessageIsDispatched]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_daemon.cpp:78]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[DaemonTest.SeveralMessagesAreDispatched]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=DaemonTest.SeveralMessagesAreDispatched]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DaemonTest.SeveralMessagesAreDispatched]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_daemon.cpp:88]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[DaemonTest.NoMessagesAreDispatchedAfterStop]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=DaemonTest.NoMessagesAreDispatchedAfterStop]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DaemonTest.NoMessagesAreDispatchedAfterStop]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_daemon.cpp:100]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[DaemonTest.StopEndsAllThreadsQuickly]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=DaemonTest.StopEndsAllThreadsQuickly]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DaemonTest.StopEndsAllThreadsQuickly]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_daemon.cpp:112]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[DaemonTest.StopWithoutStartIsHarmless]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=DaemonTest.StopWithoutStartIsHarmless]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DaemonTest.StopWithoutStartIsHarmless]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_daemon.cpp:126]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[DaemonTest.SighupReloadsAndSigtermStops]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=DaemonTest.SighupReloadsAndSigtermStops]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DaemonTest.SighupReloadsAndSigtermStops]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_daemon.cpp:133]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.ThrowsWhenSocketCannotBeCreated]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.ThrowsWhenSocketCannotBeCreated]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.ThrowsWhenSocketCannotBeCreated]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:96]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.ThrowsWhenSocketPathIsTooLong]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.ThrowsWhenSocketPathIsTooLong]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.ThrowsWhenSocketPathIsTooLong]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:101]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.SocketFileIsCreatedAndRemoved]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.SocketFileIsCreatedAndRemoved]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.SocketFileIsCreatedAndRemoved]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:109]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.OldSocketFileIsReplaced]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.OldSocketFileIsReplaced]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.OldSocketFileIsReplaced]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:118]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.PollingEmptyQueueReturnsFalse]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.PollingEmptyQueueReturnsFalse]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.PollingEmptyQueueReturnsFalse]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:130]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.ReceivesLine]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.ReceivesLine]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.ReceivesLine]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:138]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.SeveralLinesInOneWriteKeepOrder]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.SeveralLinesInOneWriteKeepOrder]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.SeveralLinesInOneWriteKeepOrder]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:150]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.LineSplitOverSeveralWrites]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.LineSplitOverSeveralWrites]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.LineSplitOverSeveralWrites]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:166]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.CarriageReturnAndEmptyLinesAreIgnored]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.CarriageReturnAndEmptyLinesAreIgnored]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.CarriageReturnAndEmptyLinesAreIgnored]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:181]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.LineWithoutNewlineIsNotReceived]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.LineWithoutNewlineIsNotReceived]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.LineWithoutNewlineIsNotReceived]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:195]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.TooLongLineClosesConnection]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.TooLongLineClosesConnection]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.TooLongLineClosesConnection]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:207]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.OtherUserIsRejected]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.OtherUserIsRejected]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.OtherUserIsRejected]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:218]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.SendReachesAllClients]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.SendReachesAllClients]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.SendReachesAllClients]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:236]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.ClientCanReconnect]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.ClientCanReconnect]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.ClientCanReconnect]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:250]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.StopWakesUpWaitForMessage]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.StopWakesUpWaitForMessage]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.StopWakesUpWaitForMessage]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:268]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.WaitForMessageAfterStopIgnoresQueuedMessages]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.WaitForMessageAfterStopIgnoresQueuedMessages]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.WaitForMessageAfterStopIgnoresQueuedMessages]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:284]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LocalSocketServerTest.StopRightAfterStartIsSafe]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LocalSocketServerTest.StopRightAfterStartIsSafe]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LocalSocketServerTest.StopRightAfterStartIsSafe]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_local_socket_server.cpp:297]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LogTest.InfoHasPriority6WithoutLocation]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LogTest.InfoHasPriority6WithoutLocation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LogTest.InfoHasPriority6WithoutLocation]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_log.cpp:37]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LogTest.WarningHasPriority4WithLocation]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LogTest.WarningHasPriority4WithLocation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LogTest.WarningHasPriority4WithLocation]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_log.cpp:44]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LogTest.ErrorHasPriority3WithLocation]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LogTest.ErrorHasPriority3WithLocation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LogTest.ErrorHasPriority3WithLocation]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_log.cpp:52]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LogTest.DebugHasPriority7WithLocation]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LogTest.DebugHasPriority7WithLocation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LogTest.DebugHasPriority7WithLocation]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_log.cpp:60]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[LogTest.EveryCallIsOneLine]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=LogTest.EveryCallIsOneLine]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[LogTest.EveryCallIsOneLine]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_log.cpp:68]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[MessageDispatcher.RebootMessageCallsReboot]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=MessageDispatcher.RebootMessageCallsReboot]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MessageDispatcher.RebootMessageCallsReboot]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_message_dispatcher.cpp:27]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[MessageDispatcher.InstallAndRemoveAppDoNotReboot]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=MessageDispatcher.InstallAndRemoveAppDoNotReboot]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MessageDispatcher.InstallAndRemoveAppDoNotReboot]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_message_dispatcher.cpp:38]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[MessageDispatcher.UnknownMessagesDoNotReboot]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=MessageDispatcher.UnknownMessagesDoNotReboot]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MessageDispatcher.UnknownMessagesDoNotReboot]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_message_dispatcher.cpp:50]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[MessageDispatcher.HeartbeatMessageResetsWatchdogTimeout]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=MessageDispatcher.HeartbeatMessageResetsWatchdogTimeout]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MessageDispatcher.HeartbeatMessageResetsWatchdogTimeout]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_message_dispatcher.cpp:64]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.EmptyStickDoesNothing]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.EmptyStickDoesNothing]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.EmptyStickDoesNothing]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:55]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.ConfigXmlIsApplied]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.ConfigXmlIsApplied]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.ConfigXmlIsApplied]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:62]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.FirmwareUpdatesPlayer]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.FirmwareUpdatesPlayer]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.FirmwareUpdatesPlayer]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:71]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.FirmwareFileNameFromConfiguration]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.FirmwareFileNameFromConfiguration]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.FirmwareFileNameFromConfiguration]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:80]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.SmilIndexIsPlayed]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.SmilIndexIsPlayed]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.SmilIndexIsPlayed]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:94]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.IndexSmilInRootIsIgnored]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.IndexSmilInRootIsIgnored]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.IndexSmilInRootIsIgnored]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:103]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.AllFilesInFixedOrder]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.AllFilesInFixedOrder]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.AllFilesInFixedOrder]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:112]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.DirectoryWithFileNameIsIgnored]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.DirectoryWithFileNameIsIgnored]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.DirectoryWithFileNameIsIgnored]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:125]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbContentHandlerTest.SymlinksAreIgnored]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbContentHandlerTest.SymlinksAreIgnored]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbContentHandlerTest.SymlinksAreIgnored]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_content_handler.cpp:134]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.AddedDeviceIsMountedAndHandled]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.AddedDeviceIsMountedAndHandled]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.AddedDeviceIsMountedAndHandled]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:51]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.MountPointIsCreated]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.MountPointIsCreated]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.MountPointIsCreated]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:68]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.SameDeviceIsMountedOnlyOnce]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.SameDeviceIsMountedOnlyOnce]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.SameDeviceIsMountedOnlyOnce]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:78]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.UnsupportedFilesystemIsNotMounted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.UnsupportedFilesystemIsNotMounted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.UnsupportedFilesystemIsNotMounted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:89]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.FailedMountIsNotHandled]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.FailedMountIsNotHandled]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.FailedMountIsNotHandled]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:101]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.FailedMountRemovesMountPoint]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.FailedMountRemovesMountPoint]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.FailedMountRemovesMountPoint]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:114]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.RemovedDeviceIsUnmounted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.RemovedDeviceIsUnmounted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.RemovedDeviceIsUnmounted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:125]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.UnknownDeviceIsNotUnmounted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.UnknownDeviceIsNotUnmounted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.UnknownDeviceIsNotUnmounted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:137]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.DestructorUnmountsAll]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.DestructorUnmountsAll]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.DestructorUnmountsAll]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:147]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.AddedUsbPartitionIsMounted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.AddedUsbPartitionIsMounted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.AddedUsbPartitionIsMounted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:163]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.AlreadyPluggedInDeviceWithoutActionIsMounted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.AlreadyPluggedInDeviceWithoutActionIsMounted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.AlreadyPluggedInDeviceWithoutActionIsMounted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:175]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.DeviceNotOnUsbIsNotMounted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.DeviceNotOnUsbIsNotMounted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.DeviceNotOnUsbIsNotMounted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:187]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.DeviceWithoutFilesystemIsNotMounted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.DeviceWithoutFilesystemIsNotMounted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.DeviceWithoutFilesystemIsNotMounted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:199]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.DeviceWithoutNodeIsIgnored]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.DeviceWithoutNodeIsIgnored]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.DeviceWithoutNodeIsIgnored]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:215]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.OtherActionsAreIgnored]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.OtherActionsAreIgnored]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.OtherActionsAreIgnored]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:227]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[UsbMonitorTest.RemoveActionUnmounts]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=UsbMonitorTest.RemoveActionUnmounts]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UsbMonitorTest.RemoveActionUnmounts]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_usb_monitor.cpp:243]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.CheckStartsPlayer]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.CheckStartsPlayer]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.CheckStartsPlayer]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:101]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.MissingPlayerUserDoesNotStartPlayer]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.MissingPlayerUserDoesNotStartPlayer]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.MissingPlayerUserDoesNotStartPlayer]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:112]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.MissingPlayerIsNoCrashAndLoggedOnce]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.MissingPlayerIsNoCrashAndLoggedOnce]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.MissingPlayerIsNoCrashAndLoggedOnce]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:136]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.PlayerInstalledLaterIsStarted]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.PlayerInstalledLaterIsStarted]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.PlayerInstalledLaterIsStarted]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:162]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.CheckDoesNotStartSecondPlayer]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.CheckDoesNotStartSecondPlayer]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.CheckDoesNotStartSecondPlayer]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:179]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.CrashedPlayerIsRestartedAfterBackoff]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.CrashedPlayerIsRestartedAfterBackoff]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.CrashedPlayerIsRestartedAfterBackoff]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:190]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.HangingPlayerIsStopped]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.HangingPlayerIsStopped]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.HangingPlayerIsStopped]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:210]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.StoppedPlayerIsStartedWithoutDelay]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.StoppedPlayerIsStartedWithoutDelay]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.StoppedPlayerIsStartedWithoutDelay]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:224]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.PlayerRunningLongEnoughResetsCrashCount]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.PlayerRunningLongEnoughResetsCrashCount]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.PlayerRunningLongEnoughResetsCrashCount]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:235]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.HeartbeatKeepsPlayerRunning]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.HeartbeatKeepsPlayerRunning]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.HeartbeatKeepsPlayerRunning]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:269]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.NoHeartbeatTimeoutMeansNoHangDetection]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.NoHeartbeatTimeoutMeansNoHangDetection]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.NoHeartbeatTimeoutMeansNoHangDetection]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:285]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.InactiveWatchdogDoesNotStartPlayer]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.InactiveWatchdogDoesNotStartPlayer]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.InactiveWatchdogDoesNotStartPlayer]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:297]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.StopPlayerStopsAllProcessesOfStartScript]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.StopPlayerStopsAllProcessesOfStartScript]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.StopPlayerStopsAllProcessesOfStartScript]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:307]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.PlayerIgnoringSigtermIsKilled]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.PlayerIgnoringSigtermIsKilled]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.PlayerIgnoringSigtermIsKilled]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:324]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.DestructorStopsPlayer]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.DestructorStopsPlayer]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.DestructorStopsPlayer]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:342]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.EnvironmentFromConfigurationIsPassed]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.EnvironmentFromConfigurationIsPassed]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.EnvironmentFromConfigurationIsPassed]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:354]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.EnvironmentFromConfigurationReplacesVariableWithSameName]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.EnvironmentFromConfigurationReplacesVariableWithSameName]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.EnvironmentFromConfigurationReplacesVariableWithSameName]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:365]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.CrashLoopRebootsAndStoresTime]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.CrashLoopRebootsAndStoresTime]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.CrashLoopRebootsAndStoresTime]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:377]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.NoRebootBeforeCrashLimit]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.NoRebootBeforeCrashLimit]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.NoRebootBeforeCrashLimit]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:390]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.NoRebootWhenDisabled]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.NoRebootWhenDisabled]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.NoRebootWhenDisabled]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:399]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.NoSecondRebootWithinInterval]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.NoSecondRebootWithinInterval]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.NoSecondRebootWithinInterval]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:408]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.RebootAgainAfterInterval]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.RebootAgainAfterInterval]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.RebootAgainAfterInterval]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:418]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.NoRebootWhenClockWentBack]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.NoRebootWhenClockWentBack]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.NoRebootWhenClockWentBack]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:428]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[WatchdogTest.NoRebootWhenTimeCannotBeStored]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=WatchdogTest.NoRebootWhenTimeCannotBeStored]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[WatchdogTest.NoRebootWhenTimeCannotBeStored]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:438]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
add_test([=[Watchdog.RestartDelayDoublesUpToMaximum]=]  /home/niko/Source/github/garlic-daemon/build/tests/garlic-tests [==[--gtest_filter=Watchdog.RestartDelayDoublesUpToMaximum]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Watchdog.RestartDelayDoublesUpToMaximum]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/niko/Source/github/garlic-daemon/tests/test_watchdog.cpp:256]==]
    WORKING_DIRECTORY [==[/home/niko/Source/github/garlic-daemon/build/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    [[TIMEOUT]] [[10]]
)
set(garlic-tests_TESTS [==[CmdParser.NoArguments]==] [==[CmdParser.OptionWithValue]==] [==[CmdParser.SeveralOptions]==] [==[CmdParser.OptionsWithoutValue]==] [==[CmdParser.LastValueWins]==] [==[CmdParser.MissingValueIsError]==] [==[CmdParser.UnknownOptionIsError]==] [==[CmdParser.ArgumentWithoutOptionIsError]==] [==[CmdParser.ProgramNameIsNotParsed]==] [==[CmdParser.HelpTextListsAllOptions]==] [==[Configuration.DefaultValues]==] [==[Configuration.ReadsAllValues]==] [==[Configuration.MissingPlayerGroupKeepsPlayerDefaults]==] [==[Configuration.MissingSingleValueKeepsDefault]==] [==[Configuration.ShippedConfigFileIsValid]==] [==[Configuration.EnvironmentEntryWithoutNameOrValueThrows]==] [==[Configuration.EnvironmentEntryWithEmptyValueIsAllowed]==] [==[Configuration.CommandLineOverridesConfigFile]==] [==[Configuration.CommandLineWithoutOptionsKeepsValues]==] [==[Configuration.HeartbeatTimeoutZeroIsAllowed]==] [==[Configuration.InvalidHeartbeatTimeoutThrows]==] [==[DaemonTest.ReceivedMessageIsDispatched]==] [==[DaemonTest.SeveralMessagesAreDispatched]==] [==[DaemonTest.NoMessagesAreDispatchedAfterStop]==] [==[DaemonTest.StopEndsAllThreadsQuickly]==] [==[DaemonTest.StopWithoutStartIsHarmless]==] [==[DaemonTest.SighupReloadsAndSigtermStops]==] [==[LocalSocketServerTest.ThrowsWhenSocketCannotBeCreated]==] [==[LocalSocketServerTest.ThrowsWhenSocketPathIsTooLong]==] [==[LocalSocketServerTest.SocketFileIsCreatedAndRemoved]==] [==[LocalSocketServerTest.OldSocketFileIsReplaced]==] [==[LocalSocketServerTest.PollingEmptyQueueReturnsFalse]==] [==[LocalSocketServerTest.ReceivesLine]==] [==[LocalSocketServerTest.SeveralLinesInOneWriteKeepOrder]==] [==[LocalSocketServerTest.LineSplitOverSeveralWrites]==] [==[LocalSocketServerTest.CarriageReturnAndEmptyLinesAreIgnored]==] [==[LocalSocketServerTest.LineWithoutNewlineIsNotReceived]==] [==[LocalSocketServerTest.TooLongLineClosesConnection]==] [==[LocalSocketServerTest.OtherUserIsRejected]==] [==[LocalSocketServerTest.SendReachesAllClients]==] [==[LocalSocketServerTest.ClientCanReconnect]==] [==[LocalSocketServerTest.StopWakesUpWaitForMessage]==] [==[LocalSocketServerTest.WaitForMessageAfterStopIgnoresQueuedMessages]==] [==[LocalSocketServerTest.StopRightAfterStartIsSafe]==] [==[LogTest.InfoHasPriority6WithoutLocation]==] [==[LogTest.WarningHasPriority4WithLocation]==] [==[LogTest.ErrorHasPriority3WithLocation]==] [==[LogTest.DebugHasPriority7WithLocation]==] [==[LogTest.EveryCallIsOneLine]==] [==[MessageDispatcher.RebootMessageCallsReboot]==] [==[MessageDispatcher.InstallAndRemoveAppDoNotReboot]==] [==[MessageDispatcher.UnknownMessagesDoNotReboot]==] [==[MessageDispatcher.HeartbeatMessageResetsWatchdogTimeout]==] [==[UsbContentHandlerTest.EmptyStickDoesNothing]==] [==[UsbContentHandlerTest.ConfigXmlIsApplied]==] [==[UsbContentHandlerTest.FirmwareUpdatesPlayer]==] [==[UsbContentHandlerTest.FirmwareFileNameFromConfiguration]==] [==[UsbContentHandlerTest.SmilIndexIsPlayed]==] [==[UsbContentHandlerTest.IndexSmilInRootIsIgnored]==] [==[UsbContentHandlerTest.AllFilesInFixedOrder]==] [==[UsbContentHandlerTest.DirectoryWithFileNameIsIgnored]==] [==[UsbContentHandlerTest.SymlinksAreIgnored]==] [==[UsbMonitorTest.AddedDeviceIsMountedAndHandled]==] [==[UsbMonitorTest.MountPointIsCreated]==] [==[UsbMonitorTest.SameDeviceIsMountedOnlyOnce]==] [==[UsbMonitorTest.UnsupportedFilesystemIsNotMounted]==] [==[UsbMonitorTest.FailedMountIsNotHandled]==] [==[UsbMonitorTest.FailedMountRemovesMountPoint]==] [==[UsbMonitorTest.RemovedDeviceIsUnmounted]==] [==[UsbMonitorTest.UnknownDeviceIsNotUnmounted]==] [==[UsbMonitorTest.DestructorUnmountsAll]==] [==[UsbMonitorTest.AddedUsbPartitionIsMounted]==] [==[UsbMonitorTest.AlreadyPluggedInDeviceWithoutActionIsMounted]==] [==[UsbMonitorTest.DeviceNotOnUsbIsNotMounted]==] [==[UsbMonitorTest.DeviceWithoutFilesystemIsNotMounted]==] [==[UsbMonitorTest.DeviceWithoutNodeIsIgnored]==] [==[UsbMonitorTest.OtherActionsAreIgnored]==] [==[UsbMonitorTest.RemoveActionUnmounts]==] [==[WatchdogTest.CheckStartsPlayer]==] [==[WatchdogTest.MissingPlayerUserDoesNotStartPlayer]==] [==[WatchdogTest.MissingPlayerIsNoCrashAndLoggedOnce]==] [==[WatchdogTest.PlayerInstalledLaterIsStarted]==] [==[WatchdogTest.CheckDoesNotStartSecondPlayer]==] [==[WatchdogTest.CrashedPlayerIsRestartedAfterBackoff]==] [==[WatchdogTest.HangingPlayerIsStopped]==] [==[WatchdogTest.StoppedPlayerIsStartedWithoutDelay]==] [==[WatchdogTest.PlayerRunningLongEnoughResetsCrashCount]==] [==[WatchdogTest.HeartbeatKeepsPlayerRunning]==] [==[WatchdogTest.NoHeartbeatTimeoutMeansNoHangDetection]==] [==[WatchdogTest.InactiveWatchdogDoesNotStartPlayer]==] [==[WatchdogTest.StopPlayerStopsAllProcessesOfStartScript]==] [==[WatchdogTest.PlayerIgnoringSigtermIsKilled]==] [==[WatchdogTest.DestructorStopsPlayer]==] [==[WatchdogTest.EnvironmentFromConfigurationIsPassed]==] [==[WatchdogTest.EnvironmentFromConfigurationReplacesVariableWithSameName]==] [==[WatchdogTest.CrashLoopRebootsAndStoresTime]==] [==[WatchdogTest.NoRebootBeforeCrashLimit]==] [==[WatchdogTest.NoRebootWhenDisabled]==] [==[WatchdogTest.NoSecondRebootWithinInterval]==] [==[WatchdogTest.RebootAgainAfterInterval]==] [==[WatchdogTest.NoRebootWhenClockWentBack]==] [==[WatchdogTest.NoRebootWhenTimeCannotBeStored]==] [==[Watchdog.RestartDelayDoublesUpToMaximum]==])
