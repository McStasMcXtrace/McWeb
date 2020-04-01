from django.test import TestCase
from django.utils import timezone
from signupper.models import Signup

class DjangoTest(TestCase):
   '''
   A simple test case to show that the testing framework works
   '''

   def test_djangotest(self):
      self.assertTrue(True)
        
class SignupTest(TestCase):
    '''
    Test model Signup
    '''
    
    
    def setUp(self):
        self.test_data = dict(
            username = 'corona',
            firstname = 'covid',
            lastname = 'sars',
            email = 'covid@sars.19',
            password = 'password'
          )
            
        
    def test_signup_str(self):
        signup = Signup(**self.test_data)
        self.assertEqual(str(signup), self.test_data['username'])
        
    def test_default_values(self):
        default_values = dict(
            institution_tpe = '',
            country = '',
            description = '',
            is_in_ldap = False,
            is_in_moodle = False,
            is_self_signup = False,
            fail_str = '',
            courses_str = '[]',
            notified = None,
            deleted = None,
        )

        timestamp_before = timezone.now()
        signup = Signup(**self.test_data)
        timestamp_after = timezone.now()

        for keyword, value in default_values.items():
            self.assertEqual(getattr(signup, keyword), value)
        
        # test created field seperately
        self.assertTrue(timestamp_before < signup.created)
        self.assertTrue(signup.created < timestamp_after)
        
        
